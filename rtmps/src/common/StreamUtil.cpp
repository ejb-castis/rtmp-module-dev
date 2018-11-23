#include "StreamUtil.hpp"
#include "NetworkUtil.hpp"
#include "StringUtil.hpp"
#include <sstream>
#include <iomanip>
#include <string.h>

namespace stream_util {

size_t get_readable_length(std::istream& stream) {
  std::streambuf* buf = stream.rdbuf();
  return buf->in_avail();
}

//size_t get_length(std::ostream& stream) {
//  return stream.tellp();
//}

void change_relative_position_istream(std::istream& stream,
                                      std::streampos relative_position) {
  std::streampos cur = stream.tellg();
  stream.seekg(cur + relative_position);
  return;
}

bool check_redable_length(std::istream &stream, size_t size) {
  if (stream_util::get_readable_length(stream) < size)
    return false;

  return true;
}

void print_istream_contents(std::istream& is, std::string& prefix) {
  std::streampos cur = is.tellg();
  size_t len = get_readable_length(is);
  char buf[len];

  is.read(buf, len);

  std::ostringstream oss;
  oss << prefix;
  std::string str_value(buf, len);
  oss << string_util::get_hex_format(str_value);
  std::cout << oss.str() << std::endl;

  is.seekg(cur);
}

void print_ostringstream_contents(std::ostringstream& os, std::string& prefix) {
  std::ostringstream oss;
  oss << prefix;
  std::string str_value = os.str();
  oss << string_util::get_hex_format(str_value);
  std::cout << oss.str() << std::endl;
}

void read_char(std::istream& is, char& readed) {
  is.read(reinterpret_cast<char*>(&readed), 1);
}

void read_short(std::istream& is, short& readed, bool use_ntoh) {
  is.read(reinterpret_cast<char*>(&readed), 2);
  if (use_ntoh)
    readed = network_util::ntoh_2(readed);
}

void read_int(std::istream& is, int& readed, bool use_ntoh) {
  is.read(reinterpret_cast<char*>(&readed), 2);
  if (use_ntoh)
    readed = network_util::ntoh_4(readed);
}

void read_double(std::istream& is, double& readed, bool use_ntoh) {
  unsigned long long read_ll_value;
  is.read(reinterpret_cast<char*>(&read_ll_value), 8);
  if (use_ntoh)
    read_ll_value = network_util::ntoh_8(read_ll_value);
  double* p = reinterpret_cast<double*>(&read_ll_value);
  readed = *p;
}

void read_uchar(std::istream& is, unsigned char& readed) {
  is.read(reinterpret_cast<char*>(&readed), 1);
}

void read_ushort(std::istream& is, unsigned short& readed, bool use_ntoh) {
  is.read(reinterpret_cast<char*>(&readed), 2);
  if (use_ntoh)
    readed = network_util::ntoh_2(readed);
}

void read_uint(std::istream& is, unsigned int& readed, bool use_ntoh) {
  is.read(reinterpret_cast<char*>(&readed), 4);
  if (use_ntoh)
    readed = network_util::ntoh_4(readed);
}

void write_char(std::ostream& os, char data, size_t& writed) {
  os.write(reinterpret_cast<char*>(&data), 1);
  ++writed;
}

void write_short(std::ostream& os, short data, bool use_hton, size_t& writed) {
  if (use_hton)
    data = network_util::hton_2(data);

  os.write(reinterpret_cast<char*>(&data), 2);
  writed = writed + 2;
}
void write_int(std::ostream& os, int data, bool use_hton, size_t& writed) {
  if (use_hton)
    data = network_util::hton_4(data);

  os.write(reinterpret_cast<char*>(&data), 4);
  writed = writed + 4;
}
void write_double(std::ostream& os, double data, bool use_hton, size_t& writed) {
  unsigned long long *long_long_ptr =
      reinterpret_cast<unsigned long long*>(&data);
  if (use_hton)
    *long_long_ptr = network_util::ntoh_8(*long_long_ptr);

  os.write(reinterpret_cast<char*>(long_long_ptr), 8);
  writed = writed + 8;
}

void write_uchar(std::ostream& os, unsigned char data, size_t& writed) {
  os.write(reinterpret_cast<char*>(&data), 1);
  ++writed;
}
void write_ushort(std::ostream& os, unsigned short data, bool use_hton, size_t& writed) {
  if (use_hton)
    data = network_util::hton_2(data);

  os.write(reinterpret_cast<char*>(&data), 2);
  writed = writed + 2;
}
void write_uint(std::ostream& os, unsigned int data, bool use_hton, size_t& writed) {
  if (use_hton)
    data = network_util::hton_4(data);

  os.write(reinterpret_cast<char*>(&data), 4);
  writed = writed + 4;
}
void write_uint_3(std::ostream& os, unsigned int data, bool use_hton, size_t& writed) {
  if (use_hton)
    data = network_util::hton_3(data);

  os.write(reinterpret_cast<char*>(&data), 3);
  writed = writed + 3;
}

}  // namespace stream_util
