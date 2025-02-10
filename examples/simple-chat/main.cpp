#include "LLMInference.h"
#include "cmd_line_parse.h"
#include <memory>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        CmdLineConfig config = parse_command_line(argc, argv);

        LLMInference llm(config.model_path, config.n_gpu_layers, config.context_size);
        llm.chat_loop();
    } catch (const std::exception& e) {
        fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }
    
    return 0;
}
