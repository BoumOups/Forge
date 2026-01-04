#pragma once

#ifdef __wasm__

#include <stddef.h>
#include <stdint.h>

extern "C" {
    void forge_host_add_executable(const char* name, size_t len, const void* sources_ptr, size_t source_count);
    int32_t forge_host_get_os();
}

struct WasmString {
    const char* ptr;
    size_t len;
};

namespace forge {
    inline size_t str_len(const char* str) {
        size_t len = 0;
        while (str[len]) len++;
        return len;
    }

    class Project {
    public:
        enum class OS { Unknown = 0, OSX = 1, Linux = 2, Windows = 3};

        template<typename... Args>
        void add_executable(const char* name, Args... sources) {
            constexpr size_t count = sizeof...(Args);
            if constexpr (count > 0) {
                const char* src_arr[] = { sources... };
                WasmString descriptor[count];
                for (size_t i = 0; i < count; ++i) {
                    descriptor[i] = {src_arr[i], str_len(src_arr[i])};
                }
                forge_host_add_executable(name, str_len(name), descriptor, count);
            } else {
                forge_host_add_executable(name, str_len(name), nullptr, 0);
            }
        }

        inline OS get_os() {
            return static_cast<OS>(forge_host_get_os());
        }
    };
}

#define FORGE_MAIN() \
    void forge_build(forge::Project& pkg); \
    extern "C" int main() { \
        forge::Project pkg; \
        forge_build(pkg); \
        return 0; \
    } \
    void forge_build(forge::Project& pkg)

#else

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
            void add_executable(std::string name, std::vector<std::string> sources) {
                targets.push_back(Target{name, "exe", sources, { "-std=c++23", "-O2"}});
            }

            template<typename... Args>
            void add_executable(std::string name, Args... sources) {
                add_executable(name, std::vector<std::string>{std::string(sources)...});
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
            namespace forge { \
                extern "C" void build(forge::Project* pkg) { \
                    ::forge_build(*pkg); \
                } \
            } \
            int main() { return 0; } \
            void forge_build(forge::Project& pkg)
}

#endif
