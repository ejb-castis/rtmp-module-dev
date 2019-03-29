#include <algorithm>
#include <bitset>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>

#include "flv_util.h"

namespace flv_util {

std::vector<unsigned char> to_vector(buffer_t const& buffer) {
  std::vector<unsigned char> v(buffer.len_);
  std::copy(buffer.ptr_, buffer.ptr_ + buffer.len_, v.begin());
  return v;
}

std::string to_string(buffer_t const& buffer) {
  std::string str =
      std::string(reinterpret_cast<char*>(buffer.ptr_), buffer.len_);
  return str;
}

unsigned char to_uchar(char const ch) {
  unsigned int val = ch & 0xFF;
  return static_cast<unsigned char>(val);
}

std::string to_hex(unsigned char const ch) {
  std::ostringstream oss;
  oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
      << static_cast<unsigned int>(ch);
  return oss.str();
}
std::string to_hex(char const ch) {
  std::ostringstream oss;
  oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
      << static_cast<unsigned int>(to_uchar(ch));
  return oss.str();
}

std::string to_hex(uint32_t value) {
  unsigned const kLF = 16;
  unsigned const kBK = 1;

  std::ostringstream oss;
  for (std::size_t n = 4; n > 0; --n) {
    oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
        << static_cast<unsigned int>((value & (0xFF << 8 * (n - 1))) >>
                                     8 * (n - 1));
    if ((n + 1) % kBK == 0 && n - 1 > 0) {
      oss << " ";
    }
    if ((n + 1) % kLF == 0 && n - 1 > 0) {
      oss << std::endl;
    }
  }
  return oss.str();
}

std::string to_hex(uint16_t value) {
  unsigned const kLF = 16;
  unsigned const kBK = 1;

  std::ostringstream oss;
  for (std::size_t n = 2; n > 0; --n) {
    oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
        << static_cast<unsigned int>((value & (0xFF << 8 * (n - 1))) >>
                                     8 * (n - 1));
    if ((n + 1) % kBK == 0 && n - 1 > 0) {
      oss << " ";
    }
    if ((n + 1) % kLF == 0 && n - 1 > 0) {
      oss << std::endl;
    }
  }
  return oss.str();
}

std::string to_hex(const char* const buffer, std::size_t buffer_len,
                   size_t showing_max_len) {
  unsigned const kLF = 16;
  unsigned const kBK = 1;

  std::ostringstream oss;
  if (showing_max_len < buffer_len) {
    oss << "only " << showing_max_len << " characters showing"
        << " out of " << buffer_len << std::endl;
  }
  for (size_t n = 0; n < std::min<std::size_t>(showing_max_len, buffer_len);
       ++n) {
    oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
        << static_cast<unsigned int>(to_uchar(buffer[n]));
    if ((n + 1) % kBK == 0 &&
        n + 1 < std::min<std::size_t>(showing_max_len, buffer_len)) {
      oss << " ";
    }
    if ((n + 1) % kLF == 0 &&
        n + 1 < std::min<std::size_t>(showing_max_len, buffer_len)) {
      oss << std::endl;
    }
  }
  return oss.str();
}

std::string to_hex(unsigned char* const buffer, std::size_t buffer_len,
                   std::size_t showing_max_len) {
  unsigned const kLF = 16;
  unsigned const kBK = 1;

  std::ostringstream oss;
  if (showing_max_len < buffer_len) {
    oss << "only " << showing_max_len << " characters showing"
        << " out of " << buffer_len << std::endl;
  }
  for (size_t n = 0; n < std::min<std::size_t>(showing_max_len, buffer_len);
       ++n) {
    oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
        << static_cast<unsigned int>(buffer[n]);
    if ((n + 1) % kBK == 0 &&
        n + 1 < std::min<std::size_t>(showing_max_len, buffer_len)) {
      oss << " ";
    }
    if ((n + 1) % kLF == 0 &&
        n + 1 < std::min<std::size_t>(showing_max_len, buffer_len)) {
      oss << std::endl;
    }
  }
  return oss.str();
}

std::string to_hex(std::vector<unsigned char>& value, size_t showing_max_len) {
  return to_hex(value.data(), value.size(), showing_max_len);
}
std::string to_hex(std::string const& value, size_t showing_max_len) {
  return to_hex(value.data(), value.size(), showing_max_len);
}
std::string to_hex(buffer_t& buffer, std::size_t showing_max_len) {
  return to_hex(buffer.ptr_, buffer.len_, showing_max_len);
}

void read_1byte(unsigned char*& buffer, unsigned char& value,
                std::size_t& len) {
  value = *buffer;
  ++buffer;
  --len;
}
void read_2byte(unsigned char*& buffer, uint16_t& value, std::size_t& len) {
  value = (buffer[0] << 8) | buffer[1];
  buffer += 2;
  len -= 2;
}
void read_3byte(unsigned char*& buffer, uint32_t& value, std::size_t& len) {
  value = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
  buffer += 3;
  len -= 3;
}
void read_4byte(unsigned char*& buffer, uint32_t& value, std::size_t& len) {
  value = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
  buffer += 4;
  len -= 4;
}
void get_1byte(unsigned char* const& src, unsigned char& value) {
  value = src[0];
}
void get_2byte(unsigned char* const& src, uint16_t& value) {
  value = 0;
  value = (src[0] << 8) | src[1];
}
void get_3byte(unsigned char* const& src, uint32_t& value) {
  value = 0;
  value = (src[0] << 16) | (src[1] << 8) | src[2];
}
void get_4byte(unsigned char* const& src, uint32_t& value) {
  value = 0;
  value = (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3];
}

void read_nbyte(unsigned char*& buffer, std::size_t n,
                unsigned char* const value, std::size_t& len) {
  unsigned char* ptr_tp_value = value;
  for (std::size_t i = 0; i < n; ++i) {
    *ptr_tp_value = *buffer;
    ++ptr_tp_value;
    ++buffer;
    --len;
  }
}

void read_nbyte(unsigned char*& buffer, std::size_t n, buffer_t& dst,
                std::size_t& len) {
  unsigned char* ptr_tp_value = dst.ptr_;
  dst.len_ = 0;
  for (std::size_t i = 0; i < n; ++i) {
    *ptr_tp_value = *buffer;
    ++ptr_tp_value;
    ++dst.len_;
    ++buffer;
    --len;
  }
}

void read_nbyte(unsigned char*& buffer, std::size_t n, buffer_t& dst) {
  unsigned char* ptr_tp_dst = dst.ptr_;
  dst.len_ = 0;
  for (std::size_t i = 0; i < n; ++i) {
    *ptr_tp_dst = *buffer;
    ++ptr_tp_dst;
    ++dst.len_;
    ++buffer;
  }
}

void read_nbyte(unsigned char* const& src, std::size_t n, buffer_t& dst) {
  unsigned char* ptr_tp_src = src;
  unsigned char* ptr_tp_dst = dst.ptr_;
  dst.len_ = 0;
  for (std::size_t i = 0; i < n; ++i) {
    *ptr_tp_dst = *ptr_tp_src;
    ++ptr_tp_dst;
    ++dst.len_;
    ++ptr_tp_src;
  }
}

void read_nbyte(buffer_t& src, buffer_t& dst) {
  unsigned char* ptr_tp_src = src.ptr_;
  unsigned char* ptr_tp_dst = dst.ptr_;
  dst.len_ = 0;
  std::size_t n = src.len_;
  for (std::size_t i = 0; i < n; ++i) {
    *ptr_tp_dst = *ptr_tp_src;
    ++ptr_tp_dst;
    ++dst.len_;
    ++ptr_tp_src;
  }
}
void read_nbyte(buffer_t& src, uint32_t& value) {
  value = 0;
  for (std::size_t n = 0; n < src.len_; ++n) {
    value |= src.ptr_[n] << (8 * (src.len_ - n - 1));
  }
}

unsigned char get_last_byte(buffer_t const& buffer) {
  return *(buffer.ptr_ + (buffer.len_ - 1));
}
void put_last_byte(buffer_t const& buffer, unsigned char value) {
  buffer.ptr_[buffer.len_ - 1] = value;
}
void put_same_nbyte(buffer_t const& buffer, unsigned char value,
                    std::size_t size) {
  for (size_t n = 0; n < size; ++n) {
    buffer.ptr_[n] = value;
  }
}
void put_nbyte(buffer_t const& buffer, buffer_t const& value) {
  for (size_t n = 0; n < value.len_; ++n) {
    buffer.ptr_[n] = value.ptr_[n];
  }
}
void put_nbyte(buffer_t const& buffer, unsigned char* const value,
               std::size_t size) {
  for (size_t n = 0; n < size; ++n) {
    buffer.ptr_[n] = value[n];
  }
}

void skip(unsigned char*& buffer, std::size_t const skip_len,
          std::size_t& len) {
  for (size_t n = 0; n < skip_len; ++n) {
    ++buffer;
    --len;
  }
}

void write_nbyte(buffer_t& src, unsigned char*& dst, std::size_t& writtenlen) {
  unsigned char* ptr_tp_src = src.ptr_;
  std::size_t n = src.len_;
  for (std::size_t i = 0; i < n; ++i) {
    *dst = *ptr_tp_src;
    ++dst;
    ++writtenlen;
    ++ptr_tp_src;
  }
}

void write_nbyte(unsigned char*& src, std::size_t n, unsigned char*& dst,
                 std::size_t& len, std::size_t& writtenlen) {
  for (std::size_t i = 0; i < n; ++i) {
    *dst = *src;
    ++dst;
    ++writtenlen;
    ++src;
    --len;
  }
}

std::vector<unsigned char> to_vector(uint8_t value) {
  std::vector<unsigned char> v;
  v.push_back(value & 0xff);
  return v;
}
std::vector<unsigned char> to_vector(uint16_t value) {
  std::vector<unsigned char> v;
  v.push_back((value & 0xff00) >> 8);
  v.push_back((value & 0x00ff));
  return v;
}

inline std::string to_string(uint32_t value) {
  unsigned char temp[4];
  temp[0] = (value & 0xff000000) >> 24;
  temp[1] = (value & 0x00ff0000) >> 16;
  temp[2] = (value & 0x0000ff00) >> 8;
  temp[3] = value & 0x000000ff;
  std::string str(reinterpret_cast<char*>(temp), 4);
  return str;
}
std::vector<unsigned char> to_vector2(uint32_t value) {
  unsigned char temp[4];
  std::memcpy(temp, &value, 4);
  std::vector<unsigned char> v;
  v.push_back(temp[3]);
  v.push_back(temp[2]);
  v.push_back(temp[1]);
  v.push_back(temp[0]);
  return v;
}
std::vector<unsigned char> to_vector(uint32_t value) {
  std::vector<unsigned char> v;
  v.push_back((value & 0xff000000) >> 24);
  v.push_back((value & 0x00ff0000) >> 16);
  v.push_back((value & 0x0000ff00) >> 8);
  v.push_back((value & 0x000000ff));
  return v;
}
std::vector<unsigned char> to_vector(uint64_t value) {
  std::vector<unsigned char> v;
  v.push_back((value & 0xff00000000000000) >> 56);
  v.push_back((value & 0x00ff000000000000) >> 48);
  v.push_back((value & 0x0000ff0000000000) >> 40);
  v.push_back((value & 0x000000ff00000000) >> 32);
  v.push_back((value & 0x00000000ff000000) >> 24);
  v.push_back((value & 0x0000000000ff0000) >> 16);
  v.push_back((value & 0x000000000000ff00) >> 8);
  v.push_back((value & 0x00000000000000ff));
  return v;
}

std::uint32_t BitReader::ReadBits(std::size_t n) {
  std::uint32_t result = 0;
  while (n) {
    auto bits_avail = 8 - (pos_ % 8);
    auto chunk_size = bits_avail >= n ? n : bits_avail;
    auto chunk_bits =
        (((std::uint32_t)(data_[pos_ / 8])) >> (bits_avail - chunk_size)) &
        ((1 << chunk_size) - 1);
    result = (result << chunk_size) | chunk_bits;
    n -= chunk_size;
    pos_ += chunk_size;
  }
  return result;
}

}  // namespace flv_util
