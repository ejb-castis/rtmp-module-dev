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

std::string to_hex(uint32_t value) {
  unsigned const kLF=16;
  unsigned const kBK=1;

  std::ostringstream oss;
  for (std::size_t n = 4; n > 0 ; --n) {
    oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
    << static_cast<unsigned int>( (value & (0xFF << 8*(n-1)) )>> 8*(n-1) );
    if ((n + 1) % kBK == 0 && n-1 > 0) {
      oss << " ";
    }
    if ((n + 1) % kLF == 0 && n-1 > 0) {
      oss << std::endl;
    }    
  }
  return oss.str();
}

std::string to_hex(unsigned char* const buffer, std::size_t buffer_len, std::size_t showing_max_len) {
  unsigned const kLF=16;
  unsigned const kBK=1;

  std::ostringstream oss;
  if ( showing_max_len < buffer_len ) {
    oss << "only " << showing_max_len << " characters showing" 
    <<" out of " << buffer_len << std::endl;
  }
  for (size_t n = 0; n < std::min<std::size_t>(showing_max_len, buffer_len); ++n) {
    oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
              << static_cast<unsigned int>(buffer[n]);
    if ((n + 1) % kBK == 0 && n+1 < std::min<std::size_t>(showing_max_len, buffer_len)) {
      oss << " ";
    }
    if ((n + 1) % kLF == 0 && n+1 < std::min<std::size_t>(showing_max_len, buffer_len)) {
      oss << std::endl;
    }
  }
  return oss.str();
}

std::string to_hex(unsigned char const* buffer, std::size_t buffer_len, std::size_t showing_max_len) {
  unsigned const kLF=16;
  unsigned const kBK=1;

  std::ostringstream oss;
  if ( showing_max_len < buffer_len ) {
    oss << "only " << showing_max_len << " characters showing" 
    <<" out of " << buffer_len << std::endl;
  }
  for (size_t n = 0; n < std::min<std::size_t>(showing_max_len, buffer_len); ++n) {
    oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
              << static_cast<unsigned int>(buffer[n]);
    if ((n + 1) % kBK == 0 && n+1 < std::min<std::size_t>(showing_max_len, buffer_len)) {
      oss << " ";
    }
    if ((n + 1) % kLF == 0 && n+1 < std::min<std::size_t>(showing_max_len, buffer_len)) {
      oss << std::endl;
    }
  }
  return oss.str();
}


}  // namespace string_util

