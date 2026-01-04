#pragma once

#include "forge.hpp"

#include <filesystem>
#include <print>
#include <sstream>

namespace forge {
class Builder {
public:
  static bool compile_build_script();
  static bool graph_validation(const Project &);
  static bool compile_project(Project &);
  static bool build_project();
};

namespace utils {
inline void ensure_directories() {
  if (!std::filesystem::exists("bin")) {
    std::filesystem::create_directory("bin");
  }
}

inline std::string join_objects(const std::vector<std::string> &objects) {
  std::ostringstream oss;
  for (size_t i = 0; i < objects.size(); i++) {
    oss << objects[i] << (i == objects.size() - 1 ? "" : " ");
  }

  return oss.str();
}

inline std::string get_compiler(const Project &project) {
  auto check = [](const char *cmd) {
#ifdef _WIN32
    const char *null_dev = "NUL";
    const char *flag = (std::string(cmd) == "cl") ? " /?" : " --version";
#else
    const char *null_dev = "/dev/null";
    const char *flag = " --version";
#endif
    std::string command = std::format("{} {} > {} 2>&1", cmd, flag, null_dev);
    return std::system(command.c_str()) == 0;
  };

  switch (project.get_compiler()) {
  case Compiler::Clang:
    if (check("clang++"))
      return "clang++";
    break;
  case Compiler::GCC:
    if (check("g++"))
      return "g++";
    break;
  case Compiler::CL:
  case Compiler::MSVC:
    if (check("cl"))
      return "cl";
    break;
  }

  std::print("❌ Compiler not found, fallback to clang++\n");
  return "clang++";
}
} // namespace utils
} // namespace forge
