#pragma once

#include <stddef.h>
#include <stdint.h>

extern "C" {
void forge_host_add_executable(const char *name, size_t len,
                               const void *sources_ptr, size_t source_count);
int32_t forge_host_get_os();

void forge_host_set_compiler(int32_t compiler);
}

struct WasmString {
  const char *ptr;
  size_t len;
};

namespace forge {

enum class Compiler { Auto = 0, Clang = 1, GCC = 2, CL = 4 };
enum class OS { Unknown = 0, OSX = 1, Linux = 2, Windows = 3 };

inline size_t str_len(const char *str) {
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

class Project {
public:
  template <typename... Args>
  void add_executable(const char *name, Args... sources) {
    constexpr size_t count = sizeof...(Args);
    if constexpr (count > 0) {
      const char *src_arr[] = {sources...};
      WasmString descriptor[count];
      for (size_t i = 0; i < count; ++i) {
        descriptor[i] = {src_arr[i], str_len(src_arr[i])};
      }
      forge_host_add_executable(name, str_len(name), descriptor, count);
    } else {
      forge_host_add_executable(name, str_len(name), nullptr, 0);
    }
  }

  inline OS get_os() { return static_cast<OS>(forge_host_get_os()); }

  inline void set_compiler(Compiler compiler) {
    forge_host_set_compiler(static_cast<int32_t>(compiler));
  }
};
} // namespace forge

#define FORGE_MAIN()                                                           \
  void forge_build(forge::Project &pkg);                                       \
  extern "C" void _start() {                                                   \
    forge::Project pkg;                                                        \
    forge_build(pkg);                                                          \
  }                                                                            \
  void forge_build(forge::Project &pkg)