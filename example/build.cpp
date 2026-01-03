#include "forge.hpp"

extern "C" void forge::build(forge::Project* pkg) {
    pkg->add_executable("test3", {"example/src/main.cpp"});
}
