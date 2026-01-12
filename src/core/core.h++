#pragma once

#include "forge.hpp"

#include <cstdio>
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
  static bool run_project(const Project &project);
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
#if defined(_WIN32)
    std::string command = std::format("cmd /c \"where {} 2>NUL\"", compiler);
    FILE *pipe = _popen(command.c_str(), "r");
    if (pipe == nullptr) {
      return false;
    }
    int exit_code = _pclose(pipe);

    forge::message::log(forge::message::Level::Info,
                        std::format("Ran command: {}, whith exit code of {}..",
                                    command, exit_code));
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
    if (check("g++"))
      return "g++";
    if (check("cl"))
      return "cl";
    break;
  case Compiler::Clang:
    return "clang++";
  case Compiler::GCC:
    return "g++";
  case Compiler::CL:
    return "cl";
  }

  message::log(message::Level::Error, "Compiler not found...");
  return "";
}
} // namespace utils
} // namespace forge
