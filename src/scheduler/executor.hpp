#pragma once

#include <future>
#include <print>
#include <string>
#include <vector>

#include "cli/log.h++"

namespace forge {
class Executor {
public:
  static bool execute_parallel(const std::vector<std::string> &commands) {
    std::vector<std::future<int>> futures;

    futures.reserve(commands.size());
    for (const auto &command : commands) {
      futures.push_back(std::async(std::launch::async, [command] {
        message::log(message::Level::Info,
                            std::format("Executing command: {}", command));
        return std::system(command.c_str());
      }));
    }

    bool success = true;
    for (auto &future : futures) {
      if (future.get() != 0)
        success = false;
    }

    return success;
  }
};
} // namespace forge
