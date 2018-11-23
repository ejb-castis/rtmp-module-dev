#ifndef RTMPHEADERPARSER_HPP_
#define RTMPHEADERPARSER_HPP_

#include <boost/logic/tribool.hpp>
#include <iostream>
#include "RtmpHeader.hpp"

namespace rtmp_protocol {

class RtmpHeaderParser {
 private:
  typedef enum {
    BASIC_HEADER_1BYTE,
    BASIC_HEADER_2BYTE,
    BASIC_HEADER_3BYTE,
    CHUNK_MSG_HEADER_TYPE0,
    CHUNK_MSG_HEADER_TYPE1,
    CHUNK_MSG_HEADER_TYPE2,
    CHUNK_MSG_HEADER_TYPE3,
    EXTENDED_TIMESTAMP,
    COMPLETE
  } parsing_state;

  parsing_state parsing_state_;
  RtmpHeader_ptr parsed_msg_;
  unsigned int connection_id_;

  parsing_state get_chunk_header_parsing_state(int header_format);

  boost::tribool parse_in_current_state(std::istream& stream, size_t buf_size,
                                                          size_t& readed_size, parsing_state state);

  boost::tribool parse_basic_header_1byte(std::istream& stream, size_t buf_size,
                                          size_t& readed_size);
  boost::tribool parse_basic_header_2byte(std::istream& stream, size_t buf_size,
                                          size_t& readed_size);
  boost::tribool parse_basic_header_3byte(std::istream& stream, size_t buf_size,
                                          size_t& readed_size);
  boost::tribool parse_chunk_header_type0(std::istream& stream, size_t buf_size,
                                            size_t& readed_size);
  boost::tribool parse_chunk_header_type1(std::istream& stream, size_t buf_size,
                                              size_t& readed_size);
  boost::tribool parse_chunk_header_type2(std::istream& stream, size_t buf_size,
                                              size_t& readed_size);
  boost::tribool parse_chunk_header_type3(std::istream& stream, size_t buf_size,
                                              size_t& readed_size);
  boost::tribool parse_extended_timestamp(std::istream& stream, size_t buf_size,
                                                size_t& readed_size);
 public:
  RtmpHeaderParser();
  virtual ~RtmpHeaderParser() {
  }
  boost::tribool parse_header(std::istream& stream, size_t buf_size,
                       size_t& readed_size);
  RtmpHeader_ptr get_parsed_msg() { return parsed_msg_; }
  void reset();
  void set_connection_id(unsigned int id) {
      connection_id_ = id;
    }
};

}  // namespace rtmp_protocol

#endif /* RTMPHEADERPARSER_HPP_ */
