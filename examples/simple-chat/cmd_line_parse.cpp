#include <iostream>
#include "cmd_line_parse.h"

void print_usage(int, char ** argv) {
    printf("\nexample usage:\n");
    printf("\n    %s -m model.gguf [-c context_size] [-ngl n_gpu_layers]\n", argv[0]);
    printf("\n");
}

CmdLineConfig parse_command_line(int argc, char* argv[]) {
    CmdLineConfig config;
    
    // parse command line arguments
    for (int i = 1; i < argc; i++) {
        try {
            if (strcmp(argv[i], "-m") == 0) {
                if (i + 1 < argc) {
                    config.model_path = argv[++i];
                } else {
                    print_usage(argc, argv);
                    throw std::runtime_error("Missing model path argument");
                }
            } else if (strcmp(argv[i], "-c") == 0) {
                if (i + 1 < argc) {
                    config.context_size = std::stoi(argv[++i]);
                } else {
                    print_usage(argc, argv);
                    throw std::runtime_error("Missing context size argument");
                }
            } else if (strcmp(argv[i], "-ngl") == 0) {
                if (i + 1 < argc) {
                    config.n_gpu_layers = std::stoi(argv[++i]);
                } else {
                    print_usage(argc, argv);
                    throw std::runtime_error("Missing GPU layers argument");
                }
            } else {
                print_usage(argc, argv);
                throw std::runtime_error("Invalid argument");
            }
        } catch (const std::exception& e) {
            fprintf(stderr, "error: %s\n", e.what());
            print_usage(argc, argv);
            throw;
        }
    }
    
    if (config.model_path.empty()) {
        print_usage(argc, argv);
        throw std::runtime_error("Model path is required");
    }
    
    return config;
}
