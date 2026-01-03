#pragma once

#include <string>
#include <vector>

namespace forge {
    struct Target {
        std::string name;
        std::string output_type;

        std::vector<std::string> sources;
        std::vector<std::string> flags;

        std::string flags_str() const {
            std:: string str;
            for (const auto& flag : flags) {
                str += flag;
                str += " ";
            }

            return str;
        }
    };

    class Project {
        public:
            void add_executable(std::string name, std::vector<std::string> sources, std::vector<std::string> flags = { "-std=c++23", "-O2"}) {
                auto target = Target{name, "exe", sources, flags};
                targets.push_back(target);
            }
            const std::vector<Target>& get_targets() const { return targets; };
        private:
            std::vector<Target> targets;
    };

    extern "C" {
        void build(forge::Project* pkg);
    }
    extern void build(Project& pkg);
}
