#include "../include/forge.hpp"

FORGE_MAIN() {
  pkg.add_executable("test4", "src/main.cpp");
  pkg.set_compiler(forge::Compiler::Clang);
}
