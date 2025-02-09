# Simple Chat

## Building

- Assuming llama.cpp has been cloned and built/installed into the directory of `~/Projects/llama.cpp`. Please change the actural directory accordingly.

```sh
cd examples/simple-chat
cmake -S . -B build -DCMAKE_PREFIX_PATH=~/Projects/llama.cpp/inst/lib/cmake
cmake --build build
```

## Run simple-chat

- You may change the model path accordingly.
- The `DeepSeek-R1-Distill-Llama-8B-Q4_K_M.gguf` can be downloaded [here](https://huggingface.co/unsloth/DeepSeek-R1-Distill-Llama-8B-GGUF/tree/main)

```
./build/llama-simple-chat -m ~/Projects/models/DeepSeek-R1-Distill-Llama-8B-Q4_K_M.gguf
```