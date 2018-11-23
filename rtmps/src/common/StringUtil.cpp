#include "StringUtil.hpp"
#include <sstream>
#include <iomanip>

namespace string_util {

std::string get_hex_format(std::string& value) {
  std::ostringstream oss;
  size_t len = value.length();
  oss << std::hex << std::setfill('0');
  for (size_t i = 0; i < len; i++) {
    oss << std::setw(2) << static_cast<unsigned int>(get_unsigned(value.c_str()[i])) << " ";
  }
  return oss.str();
}

unsigned char get_unsigned(const char ch) {
  unsigned int val = ch & 0xFF;
  return static_cast<unsigned char>(val);
}

}  // namespace string_util

