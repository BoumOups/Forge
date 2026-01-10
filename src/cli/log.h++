#pragma once

#include <print>
#include <string>

namespace forge {
namespace message {
enum class Level { Info, Error };

inline void log(Level level, const std::string &msg) {
  switch (level) {
  case Level::Info:
    std::println("[INFO] {}", msg);
    break;
  case Level::Error:
    std::println("[ERROR] {}", msg);
    break;
  }
#if defined(_WIN32)
  std::fflush(stdout);
#endif
}
} // namespace message
} // namespace forge