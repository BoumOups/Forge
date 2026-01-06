#pragma once

#include "forge.hpp"

namespace forge {
namespace loader {
bool load_and_run_wasm_script(forge::Project &project,
                              const std::string &wasm_path);
}
} // namespace forge
