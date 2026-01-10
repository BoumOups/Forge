#include "core/paths.h++"

#include <filesystem>
#include <print>
#include <string_view>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <limits.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

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

std::filesystem::path forge::Path::get_forge_path() {
#if defined(_WIN32)
  wchar_t path[MAX_PATH];
  GetModuleFileNameW(NULL, path, MAX_PATH);
  return std::filesystem::path(path).parent_path().parent_path();
#elif defined(__APPLE__)
  char path[PATH_MAX];
  uint32_t size = sizeof(path);
  if (_NSGetExecutablePath(path, &size) != 0) {
    return std::filesystem::current_path();
  }

  char resolved_path[PATH_MAX];
  if (realpath(path, resolved_path) != nullptr) {
    return std::filesystem::path(resolved_path).parent_path().parent_path();
  }

  return std::filesystem::path(path).parent_path().parent_path();
#else
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  return std::filesystem::path(std::string(result, (count > 0) ? count : 0))
      .parent_path()
      .parent_path();
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
  std::filesystem::path clang_path =
      get_forge_path() / std::filesystem::path(WASI_BASE_PATH.data()) /
      std::string(get_platform_suffix()) / "bin/clang++";
  return std::filesystem::absolute(clang_path);
}
} // namespace forge