#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace forge {
    struct Step {
      std::string name;
      std::string command;

      std::vector<std::filesystem::path> inputs;
      std::vector<std::filesystem::path> outputs;

      std::string hash_key;
    };

    struct Target {
        std::string name;
        enum class Type { Executable, StaticLib, SharedLib };

        std::vector<std::filesystem::path> sources;

        std::vector<std::string> compile_flags;
        std::vector<std::string> link_flags;

        std::vector<Target*> dependencies;
    };

    struct BuildGraph {
        std::vector<Target> targets;
        std::vector<Step> plan;
    };
}
