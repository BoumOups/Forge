#pragma once

#include <filesystem>
#include <streambuf>
#include <string>

namespace forge {
namespace paths {
inline std::string get_output_directory() {
  std::filesystem::path output_dir =
      std::filesystem::current_path() / "forge-out";
  return std::string(output_dir.string());
}
} // namespace paths
} // namespace forge