#pragma once

#include <filesystem>

namespace forge {

class Path {
private:
  static constexpr std::string_view WASI_BASE_PATH = "vendor/wasi-sdk/";

private:
  static bool ensure_directory_exists(const std::filesystem::path &path);
  static const std::string_view get_platform_suffix();
  static std::filesystem::path get_forge_path();

public:
  static std::filesystem::path get_output_directory_path();
  static std::filesystem::path get_build_script_path();
  static std::filesystem::path get_wasm_compiler_path();
};
} // namespace forge