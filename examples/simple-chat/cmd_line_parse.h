#include <iostream>

struct CmdLineConfig {
    std::string model_path;
    int n_gpu_layers;
    int context_size;
    
    // Constructor with default values
    CmdLineConfig() : model_path(""), n_gpu_layers(99), context_size(2048) {}
};

void print_usage(int, char ** argv);

CmdLineConfig parse_command_line(int argc, char* argv[]);