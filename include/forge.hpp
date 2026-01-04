#pragma once

#ifdef __wasm__

#include <cstdint>
#include <string>
#include <vector>

extern "C" {
    void forge_host_add_executable(const char* name, size_t len, const void* sources_ptr, size_t source_count);
    int32_t forge_host_get_os();
}

struct WasmString {
    const char* ptr;
    size_t len;
};

namespace forge {
    class Project {
    public:
        enum class OS = { Unknown = 0, OSX = 1, Linux = 2, Windows = 3};

        void add_executable(std::string name) {
            std::vector<WasmString> descriptor;
            for (const auto& source : sources) (
                descriptor.push_back({source.c_str, source.lenght()});
            )
            forge_host_add_executable(name.c_str(), name.length(), descriptor.data(), descriptor.size());
        }

        inline OS get_os() {
            return static_cast<OS>(forge_host_get_os());
        }
    };
}

#define FORGE_MAIN() \
    extern "C" int main() { \
        forge::Project pkg; \
        forge_build(pkg); \
        return 0; \
    } \
    void forge_build(forge::Project& pkg)

#else

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

    #define FORGE_MAIN() \
            void forge_build(forge::Project& pkg); \
            int main() { return 0; } \
            void forge_build(forge::Project& pkg)
}

#endif
