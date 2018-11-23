#ifndef STREAMUTIL_HPP_
#define STREAMUTIL_HPP_

#include <iostream>
#include <sstream>

namespace stream_util {
size_t get_readable_length(std::istream& is);
//size_t get_length(std::ostream& os);
void change_relative_position_istream(std::istream& stream,
                                      std::streampos relative_position);
bool check_redable_length(std::istream& is, size_t size);
void print_istream_contents(std::istream& is, std::string& prefix);
void print_ostringstream_contents(std::ostringstream& os, std::string& prefix);

// need stream size checking
void read_char(std::istream& is, char& readed);
void read_short(std::istream& is, short& readed, bool use_ntoh);
void read_int(std::istream& is, int& readed, bool use_ntoh);
void read_double(std::istream& is, double& readed, bool use_ntoh);

void read_uchar(std::istream& is, unsigned char& readed);
void read_ushort(std::istream& is, unsigned short& readed, bool use_ntoh);
void read_uint(std::istream& is, unsigned int& readed, bool use_ntoh);

void write_char(std::ostream& os, char data, size_t& writed);
void write_short(std::ostream& os, short data, bool use_hton, size_t& writed);
void write_int(std::ostream& os, int data, bool use_hton, size_t& writed);
void write_double(std::ostream& os, double data, bool use_hton, size_t& writed);

void write_uchar(std::ostream& os, unsigned char data, size_t& writed);
void write_ushort(std::ostream& os, unsigned short data, bool use_hton, size_t& writed);
void write_uint(std::ostream& os, unsigned int data, bool use_hton, size_t& writed);
void write_uint_3(std::ostream& os, unsigned int data, bool use_hton, size_t& writed);

}  // namespace stream_util

#endif /* STREAMUTIL_HPP_ */
