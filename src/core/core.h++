#pragma once

#include "forge.hpp"

#include <cstdlib>
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
  auto check = [](const char *compiler) {
#ifdef _WIN32
    std::string command = std::format("where {} 2>NUL", command);
    FILE *pipe = _popen(command.c_str(), "r");
    if (pipe == nullptr) {
      return false;
    }
    int exit_code = _pclose(pipe);
    return exit_code == 0;
#else
    std::string full_command =
        std::format("command -v {} > /dev/null 2>&1", compiler);
    return std::system(full_command.c_str()) == 0;
#endif
  };

  switch (project.get_compiler()) {
  case Compiler::Auto:
    if (check("clang++"))
      return "clang++";
    else if (check("g++"))
      return "g++";
    else if (check("cl"))
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
