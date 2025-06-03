#include <utils/conversionUtils.hpp>

std::vector<std::uint8_t> ConversionUtils::str2bytes(std::string str) {
  std::vector<std::uint8_t> vector(str.begin(), str.end());

  return vector;
}

std::string ConversionUtils::bytes2str(std::vector<std::uint8_t> str) {
  std::string str1(str.begin(), str.end());
  return str1;
}
