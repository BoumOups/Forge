#pragma once

#include "forge.hpp"


namespace forge::loader {
bool load_and_run_wasm_script(Project &project,
                              const std::string &wasm_path);
}

