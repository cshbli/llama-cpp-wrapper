#ifndef LLMINFERENCE_H
#define LLMINFERENCE_H

#include "llama.h"
#include <string>
#include <vector>

// Created an LLMInference class to encapsulate all LLM-related functionality
class LLMInference {
// The default access specifier is private.
// For structs, the default access specifier is public.
private:
    llama_context   *ctx;
    llama_model     *model;
    llama_sampler   *sampler;
    const llama_vocab   *vocab;
    std::vector<llama_chat_message> messages;
    std::vector<char> formatted;
    int prev_len;
    
    // Configuration parameters
    std::string model_path;
    int n_gpu_layers;
    int context_size;

    void initialize();
    void process_message(const std::string& user_input);
    std::string generate(const std::string& prompt);    
    void cleanup();

public:
    LLMInference(const std::string& modelPath, int ngl = 99, int n_ctx = 2048);
    ~LLMInference();
    void chat_loop();    
};

#endif