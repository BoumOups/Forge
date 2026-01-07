#pragma once

#include <filesystem>
#include <string>

namespace forge {
namespace paths {
inline std::string get_output_directory_path() {
  std::filesystem::path output_dir =
      std::filesystem::current_path() / "forge-out";
  return std::string(output_dir.string());
}

inline std::string get_build_script_path() {
  std::filesystem::path script_path =
      std::filesystem::current_path() / "build.cpp";
  return std::string(script_path.string());
}

inline std::string get_wasm_compiler_path() {
#if defined(_WIN32) || defined(_WIN64)
  std::filesystem::path clang_path =
      "../vendor/wasi-sdk/wasi-sdk-29.0-x86_64-windows/bin/clang++";
#endif
#if defined(__linux__)
  std::filesystem::path clang_path =
      "../vendor/wasi-sdk/wasi-sdk-29.0-x86_64-linux/bin/clang++";
#endif
#if defined(__APPLE__)
  std::filesystem::path clang_path =
      "../vendor/wasi-sdk/wasi-sdk-29.0-ARM64-apple/bin/clang++";
#endif
  return clang_path.string();
}
} // namespace paths
} // namespace forge