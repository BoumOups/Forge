#pragma once

#include <cstdlib>
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

    for (const auto command : commands) {
      futures.push_back(std::async(std::launch::async, [command] {
        std::print("[Run] {}\n", command);
        forge::message::log(forge::message::Level::Info,
                            std::format("Executing command: {}", command));
        return std::system(command.c_str());
      }));
    }

    bool success = true;
    for (auto &futur : futures) {
      if (futur.get() != 0)
        success = false;
    }

    return success;
  }
};
} // namespace forge
