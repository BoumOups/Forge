#pragma once

#include "forge.hpp"

#include <print>
#include <sstream>

#include "cli/log.h++"

namespace forge {

class Builder {
public:
  static bool compile_build_script();
  static bool graph_validation(const Project &);
  static bool compile_project(Project &);
  static bool run_project(Project &project);
  static bool clean_project();
  static bool build_project(bool auto_execute = false);
};

namespace utils {
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
  case Compiler::Auto:
    if (check("clang++"))
      return "clang++";
    if (check("g++"))
      return "g++";
    if (check("cl"))
      return "cl";
    break;
  case Compiler::Clang:
    return "clang++";
    break;
  case Compiler::GCC:
    return "g++";
    break;
  case Compiler::CL:
    return "cl";
    break;
  }

  forge::message::log(message::Level::Error, "Compiler not found...");
  return "";
}
} // namespace utils
} // namespace forge
