cmake_minimum_required(VERSION 3.12)
project(llama-simple-chat)

set(TARGET llama-simple-chat)

find_package(Llama REQUIRED)

add_executable(${TARGET} ${CMAKE_CURRENT_LIST_DIR}/main.cpp ${CMAKE_CURRENT_LIST_DIR}/cmd_line_parse.cpp ${CMAKE_CURRENT_LIST_DIR}/../../src/LLMInference.cpp)
# Add the external include directory
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include)
install(TARGETS ${TARGET} RUNTIME)
target_link_libraries(${TARGET} PRIVATE llama ggml::all ${CMAKE_THREAD_LIBS_INIT})
target_compile_features(${TARGET} PRIVATE cxx_std_17)