#include "forge.hpp"

#include <dlfcn.h>
#include <cstdlib>
#include <print>

typedef void (*build_t)(forge::Project*);

bool load_and_run_script(forge::Project& pkg) {
    std::string compile_cmd = "clang++ -std=c++23 -dynamiclib example/build.cpp "
                                  "-I./include -o bin/build.dylib "
                                  "-undefined dynamic_lookup";

    if (std::system(compile_cmd.c_str()) != 0) {
        std::print("❌ Failed to compile build.cpp\n");
        return false;
    }

    void* handle = dlopen("bin/build.dylib", RTLD_NOW);
    if (!handle) {
        std::print("❌ dlsym error: {}\n", dlerror());
        dlclose(handle);
        return false;
    }

    auto entry = (build_t)dlsym(handle, "build");
    if (entry) {
        entry(&pkg);
    }

    dlclose(handle);
    return true;
}
