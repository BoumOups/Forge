#pragma once

#include <cstdlib>
#include <future>
#include <string>
#include <vector>
#include <print>

namespace forge {
    class Executor {
        public:
        static bool execute_parallel(const std::vector<std::string>& commands) {
            std::vector<std::future<int>> futures;

            for (const auto command : commands) {
                futures.push_back(std::async(std::launch::async, [command]{
                    std::print("[Run] {}\n", command);
                    return std::system(command.c_str());
                }));
            }

            bool success = true;
            for (auto& futur : futures) {
                if (futur.get() != 0) success = false;
            }

            return success;
        }
    };
}
