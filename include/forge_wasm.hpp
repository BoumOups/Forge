#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stddef.h>
#include <stdint.h>

extern "C" {
void forge_host_add_executable(const char *name, size_t len,
                               const void *sources_ptr, size_t source_count,
                               const void *flags_ptr, size_t flags_count);
void forge_host_set_compiler(int32_t compiler);

int32_t forge_host_get_os();
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
  static void add_executable(const char *name,
                             const std::initializer_list<const char *> &sources,
                             const std::initializer_list<const char *> &flags) {
    const size_t src_count = std::size(sources);
    const size_t flags_count = std::size(flags);

    WasmString src_descriptor[src_count];
    WasmString flags_descriptor[flags_count];

    if (src_count > 0) {
        size_t i = 0;
      for (const char* source : sources) {
        src_descriptor[i] = {source, str_len(source)};
        i++;
      }
    }

    if (flags_count > 0) {
        size_t i = 0;
      for (const char *flag : flags) {
        flags_descriptor[i] = {flag, str_len(flag)};
        i++;
      }
    }
    forge_host_add_executable(name, str_len(name), src_descriptor, src_count,
                              flags_descriptor, flags_count);
  }

  static inline OS get_os() { return static_cast<OS>(forge_host_get_os()); }

  static inline void set_compiler(Compiler compiler) {
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
