#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ConversionUtils {
std::vector<std::uint8_t> str2bytes(std::string str);

std::string bytes2str(std::vector<std::uint8_t> str);
} // namespace ConversionUtils
