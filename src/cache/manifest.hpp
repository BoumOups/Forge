#pragma once

#include "../../vendor/json/json.hpp"
#include <fstream>

namespace forge {
    class Manifest {
        public:
        static nlohmann::json load() {
            std::ifstream file("bin/manifest.json");
            if (!file.is_open()) return nlohmann::json::object();

            return nlohmann::json::parse(file);
        }

        static void save(const nlohmann::json& data) {
            std::ofstream file("bin/manifest.json");
            file << data.dump();
        }
    };
}
