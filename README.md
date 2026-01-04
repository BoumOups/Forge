# Forge

**Forge** is a modern C++ build system that allows you to write your build scripts in C++. It leverages WebAssembly (WASM) to execute build logic securely and portably.

**Important**: This project is in its early stages and is not yet ready for production use, and is also only usable on MacOS and will be soon available on Linux and Windows.

## Features

- **C++ Build Scripts**: Write your build configuration in C++23.
- **WASM-Powered**: Build scripts are compiled to WASM and executed by the Forge runner.
- **Simple API**: Define targets and sources with a minimal, intuitive API.

## Building Forge

To build the Forge tool itself from source:

```bash
mkdir build
cd build
cmake ..
make
```

Prerequisites:
- CMake
- Clang (with WASM support / WASI-SDK)
- OpenSSL

## Usage

### 1. Create a Build Script

Create a `build.cpp` file in your project root:

```cpp
#include "forge.hpp"

FORGE_MAIN() {
    // Define an executable target 'app' with source 'src/main.cpp'
    pkg.add_executable("app", "src/main.cpp");
}
```

### 2. Build Your Project

Run the `forge` tool to compile your project:

```bash
/path/to/forge build
```

This will:
1. Compile your `build.cpp` into a WASM module.
2. Execute the WASM module to discover targets.
3. Compile and link your C++ sources.

### Commands

- `forge build` : Compile the current project.
- `forge run`   : Build and execute the target (not implemented yet).
- `forge clean` : Remove build artifacts (not implemented yet).
