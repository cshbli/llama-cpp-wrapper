#include "LLMInference.h"
#include <cstring>
#include <iostream>

LLMInference::LLMInference(const std::string& modelPath, int ngl, int n_ctx)
: model_path(modelPath), n_gpu_layers(ngl), context_size(n_ctx) {
    initialize();
}

LLMInference::~LLMInference() {
    cleanup();
}

void LLMInference::initialize() {
    // Set up logging
    llama_log_set([](enum ggml_log_level level, const char* text, void*) {
        if (level >= GGML_LOG_LEVEL_ERROR) {
            fprintf(stderr, "%s", text);
        }
    }, nullptr);

    ggml_backend_load_all();

    // Initialize model
    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = n_gpu_layers;

    model = llama_model_load_from_file(model_path.c_str(), model_params);
    if (!model) {
        throw std::runtime_error("Unable to load model");
    }

    vocab = llama_model_get_vocab(model);

    // Initialize context
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = context_size;
    ctx_params.n_batch = context_size;

    ctx = llama_init_from_model(model, ctx_params);
    if (!ctx) {
        throw std::runtime_error("Failed to create the llama_context");
    }

    // Initialize sampler
    sampler = llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(sampler, llama_sampler_init_min_p(0.05f, 1));
    llama_sampler_chain_add(sampler, llama_sampler_init_temp(0.8f));
    llama_sampler_chain_add(sampler, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

    formatted.resize(llama_n_ctx(ctx));
}

void LLMInference::cleanup() {
    for (auto& msg : messages) {
        free(const_cast<char*>(msg.content));
    }
    llama_sampler_free(sampler);
    llama_free(ctx);
    llama_model_free(model);
}

std::string LLMInference::generate(const std::string& prompt) {
    std::string response;

    const bool is_first = llama_get_kv_cache_used_cells(ctx) == 0;

    // tokenize the prompt
    const int n_prompt_tokens = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), NULL, 0, is_first, true);
    std::vector<llama_token> prompt_tokens(n_prompt_tokens);
    if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), is_first, true) < 0) {
        GGML_ABORT("failed to tokenize the prompt\n");
    }

    // prepare a batch for the prompt
    llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());
    llama_token new_token_id;
    while (true) {
        // check if we have enough space in the context to evaluate this batch
        int n_ctx = llama_n_ctx(ctx);
        int n_ctx_used = llama_get_kv_cache_used_cells(ctx);
        if (n_ctx_used + batch.n_tokens > n_ctx) {
            printf("\033[0m\n");
            fprintf(stderr, "context size exceeded\n");
            exit(0);
        }

        if (llama_decode(ctx, batch)) {
            GGML_ABORT("failed to decode\n");
        }

        // sample the next token
        new_token_id = llama_sampler_sample(sampler, ctx, -1);

        // is it an end of generation?
        if (llama_vocab_is_eog(vocab, new_token_id)) {
            break;
        }

        // convert the token to a string, print it and add it to the response
        char buf[256];
        int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
        if (n < 0) {
            GGML_ABORT("failed to convert token to piece\n");
        }
        std::string piece(buf, n);
        printf("%s", piece.c_str());
        fflush(stdout);
        response += piece;

        // prepare the next batch with the sampled token
        batch = llama_batch_get_one(&new_token_id, 1);
    }

    return response;
};

void LLMInference::process_message(const std::string& user_input) {
    const char* tmpl = llama_model_chat_template(model, nullptr);
        
    messages.push_back({"user", strdup(user_input.c_str())});
    int new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), 
                                            true, formatted.data(), formatted.size());
    
    if (new_len > (int)formatted.size()) {
        formatted.resize(new_len);
        new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), 
                                            true, formatted.data(), formatted.size());
    }
    
    if (new_len < 0) {
        throw std::runtime_error("Failed to apply the chat template");
    }

    // remove previous messages to obtain the prompt to generate the response
    std::string prompt(formatted.begin() + prev_len, formatted.begin() + new_len);

    printf("\033[33m"); // yellow color for response
    std::string response = generate(prompt);
    printf("\n\033[0m"); // reset color

    messages.push_back({"assistant", strdup(response.c_str())});
    prev_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), false, nullptr, 0);
    
    if (prev_len < 0) {
        throw std::runtime_error("Failed to apply the chat template");
    }    
}

void LLMInference::chat_loop() {
    while (true) {
        printf("\033[32m> \033[0m");    // green color for user input
        std::string user;
        std::getline(std::cin, user);

        if (user.empty()) {
            break;
        }

        process_message(user);
    }        
}