#pragma once

#include "../../vendor/json/json.hpp"
#include <fstream>

namespace forge {
class Manifest {
public:
  static nlohmann::json load() {
    std::ifstream file("forge-out/manifest.json");
    if (!file.is_open())
      return nlohmann::json::object();

    return nlohmann::json::parse(file);
  }

  static void save(const nlohmann::json &data) {
    std::ofstream file("forge-out/manifest.json");
    file << data.dump();
  }
};
} // namespace forge
