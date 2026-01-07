#pragma once

#include <string>

namespace forge {
namespace message {
enum class Level { Info, Error };

void log(Level level, const std::string &msg);
} // namespace message
} // namespace forge