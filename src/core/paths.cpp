#include "core/paths.h++"

namespace forge {
bool Path::ensure_directory_exists(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    return std::filesystem::create_directories(path);
  }
  return true;
}

const std::string_view Path::get_platform_suffix() {
#if defined(_WIN32) || defined(_WIN64)
  return "wasi-sdk-29.0-x86_64-windows";
#endif
#if defined(__linux__)
  return "wasi-sdk-29.0-x86_64-linux";
#endif
#if defined(__APPLE__)
  return "wasi-sdk-29.0-ARM64-apple";
#endif
}

std::filesystem::path Path::get_output_directory_path() {
  std::filesystem::path output_dir =
      std::filesystem::current_path() / "forge-out";
  ensure_directory_exists(output_dir);
  return output_dir;
}

std::filesystem::path Path::get_build_script_path() {
  std::filesystem::path script_path =
      std::filesystem::current_path() / "build.cpp";
  return script_path;
}

std::filesystem::path Path::get_wasm_compiler_path() {
  std::filesystem::path clang_path = WASI_BASE_PATH.data() +
                                     std::string(get_platform_suffix()) +
                                     "/bin/clang++";
  return clang_path;
}
} // namespace forge