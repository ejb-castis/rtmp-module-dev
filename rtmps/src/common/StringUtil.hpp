#ifndef STRINGUTIL_HPP_
#define STRINGUTIL_HPP_
#include <string>

namespace string_util {

std::string get_hex_format(std::string& value);
unsigned char get_unsigned(const char ch);
std::string to_hex(uint32_t value);
std::string to_hex(unsigned char* const buffer, std::size_t buffer_len, std::size_t showing_max_len=32);
std::string to_hex(unsigned char const* buffer, std::size_t buffer_len, std::size_t showing_max_len=32);
}  // namespace string_util

#endif /* STRINGUTIL_HPP_ */
