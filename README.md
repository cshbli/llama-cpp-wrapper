# llama.cpp Class Encapsulation and Wrapper

## Deliberate design decision for C Interface
1. Ensures Compatibility

A C interface ensures compatibility with a broader range of programming languages and tools, as C APIs are:

- <b>Easily callable</b> from other languages such as Python, Java, Rust, or even C++ itself.
- Universally supported in cross-language bindings and FFI (Foreign Function Interfaces).

For example:

- Libraries with a C API can be easily wrapped for Python (e.g., via `ctypes` or `cffi`).
- They are also compatible with systems or frameworks that require C-style linkage.
In contrast, C++ classes and features like inheritance, templates, or virtual functions can be complex to map to other languages.

2. Simplified Binary Interface (ABI Stability)

C has a more stable ABI (Application Binary Interface) compared to C++:

- Different C++ compilers (e.g., GCC, Clang, MSVC) often produce incompatible name mangling for symbols, making binaries less portable.
- By exposing a flat C API, llama.cpp avoids these issues and ensures consistent symbol names, even when used with different compilers or environments.

3. Easier for Embedding in Low-Level Systems

C interfaces are ideal for:

- Embedding the library in low-level environments like game engines, operating systems, or hardware platforms where C is dominant.
- C++ abstractions like exceptions or RTTI (Run-Time Type Information) can add overhead and complexity, which may not be desirable for performance-critical applications like llama.cpp.

4. Lightweight and Minimalist Design

A C interface aligns with the lightweight philosophy of llama.cpp:

- It avoids introducing complex object-oriented abstractions that might make the code harder to understand, debug, or optimize.
- A flat procedural API is easier for developers to trace and use in performance-critical scenarios like machine learning inference.

5. User-Controlled Encapsulation

Instead of enforcing encapsulation through C++ classes, llama.cpp lets developers implement their own abstractions:

- A user of the library can wrap the C API in their preferred C++ classes or use it directly, giving them flexibility over how the library is integrated.