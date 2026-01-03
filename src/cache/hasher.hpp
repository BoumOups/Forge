#pragma once

#include <fstream>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

#include <openssl/sha.h>

namespace forge {
    class Hasher {
        public:
        static std::string hash_file(const std::string& path, const std::string& extra_data) {
            std::ifstream file(path, std::ios::binary);
            if (!file) return "";

            std::stringstream buffer;
            buffer << file.rdbuf() << extra_data;
            std::string content = buffer.str();

            unsigned char hash[SHA_DIGEST_LENGTH];
            SHA1(reinterpret_cast<const unsigned char*>(content.c_str()), content.length(), hash);

            std::stringstream ss;
            for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
                ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
            }

            return ss.str();
        }
    };
}
