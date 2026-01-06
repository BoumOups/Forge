#pragma once

#include <fstream>
#include <iomanip>
#include <ios>
#include <print>
#include <sstream>
#include <string>

#include <a5hash/a5hash.h>

namespace forge {
class Hasher {
public:
  static std::string hash_file(const std::string &path,
                               const std::string &extra_data) {
    std::ifstream file(path, std::ios::binary);
    if (!file)
      return "";

    std::stringstream buffer;
    buffer << file.rdbuf() << extra_data;
    std::string content = buffer.str();

    uint64_t hash = a5hash_impl::a5hash(
        reinterpret_cast<const unsigned char *>(content.c_str()),
        content.length(), 0);

    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << hash;
    std::print("Computed hash for file {}: {}\n", path, ss.str());

    return ss.str();
  }
};
} // namespace forge
