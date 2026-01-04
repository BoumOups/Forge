#pragma once

#include "forge.hpp"

#include <filesystem>
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

inline std::string get_compiler() {
#if defined(_WIN32)
  std::string check_cmd = "where cl";
  if (std::system(check_cmd.c_str()) == 0) {
    return "cl";
  }
#else
  std::string check_cmd = "which clang++";
  if (std::system(check_cmd.c_str()) == 0) {
    return "clang++";
  }
#endif

  return "clang++";
}
} // namespace utils
} // namespace forge
