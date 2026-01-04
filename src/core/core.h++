#pragma once

#include "forge.hpp"

#include <sstream>
#include <filesystem>

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
        return oss.str();
      }

      return "";
    }
  }
}
