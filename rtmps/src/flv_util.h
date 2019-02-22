#pragma once
#include <string>
#include <vector>

namespace flv_util {

struct buffer_t{
public:
  buffer_t(std::size_t max_len) :
    ptr_(new unsigned char[max_len])
  {
    max_len_ = max_len;
  }
  ~buffer_t() {
    delete []ptr_;
  }
  unsigned char* const ptr_{nullptr};
  std::size_t len_{0};
  std::size_t max_len_{0};
};
std::vector<unsigned char> to_vector(buffer_t const& buffer);
std::vector<unsigned char> to_vector(uint8_t value);
std::vector<unsigned char> to_vector(uint16_t value);
std::vector<unsigned char> to_vector(uint32_t value);
std::vector<unsigned char> to_vector(uint64_t value);
std::vector<unsigned char> to_vector2(uint32_t value);
std::string to_string(uint32_t value);
std::string to_string(buffer_t const& buffer);
unsigned char to_uchar(char const ch);
std::string to_hex(std::vector<unsigned char>& value, std::size_t showing_max_len=32);
std::string to_hex(unsigned char const ch);
std::string to_hex(char const ch);
std::string to_hex(uint32_t value);
std::string to_hex(uint16_t value);
std::string to_hex(std::string const& value, size_t showing_max_len=32);
std::string to_hex(char* const buffer, std::size_t buffer_len, size_t showing_max_len=32);
std::string to_hex(unsigned char* const buffer, std::size_t buffer_len, std::size_t showing_max_len=32);
std::string to_hex(buffer_t& buffer, std::size_t showing_max_len=32);
void read_1byte(unsigned char*& buffer, unsigned char& value, std::size_t& len);
void read_2byte(unsigned char*& buffer, uint16_t& value, std::size_t& len);
void read_3byte(unsigned char*& buffer, uint32_t& value, std::size_t& len);
void read_4byte(unsigned char*& buffer, uint32_t& value, std::size_t& len);
void get_1byte(unsigned char* const & src, unsigned char& value);
void get_2byte(unsigned char* const & src, uint16_t& value);
void get_3byte(unsigned char* const & src, uint32_t& value);
void get_4byte(unsigned char* const & src, uint32_t& value);
void read_nbyte(unsigned char*& buffer, std::size_t n, unsigned char* const value, std::size_t& len);
void read_nbyte(unsigned char*& buffer, std::size_t n, buffer_t& dst, std::size_t& len);
void read_nbyte(unsigned char*& buffer, std::size_t n, buffer_t& dst);
void read_nbyte(unsigned char* const & src, std::size_t n, buffer_t& dst);
void read_nbyte(buffer_t& src, buffer_t& dst); 
void read_nbyte(buffer_t& src, uint32_t& value);
unsigned char get_last_byte(buffer_t const&  buffer);
void put_last_byte(buffer_t const&  buffer, unsigned char value);
void put_same_nbyte(buffer_t const&  buffer, unsigned char value, std::size_t size);
void put_nbyte(buffer_t const&  buffer, buffer_t const& value);
void put_nbyte(buffer_t const&  buffer, unsigned char* const value, std::size_t size);
void skip(unsigned char*& buffer, std::size_t const skip_len, std::size_t& len);
void write_nbyte(buffer_t& src, unsigned char*& dst, std::size_t& writtenlen); 
void write_nbyte(unsigned char*& src, std::size_t n, unsigned char*& dst, std::size_t& len, std::size_t& writtenlen);

}