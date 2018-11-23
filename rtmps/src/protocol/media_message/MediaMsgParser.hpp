#ifndef VIDEOMESSAGEPARSER_HPP_
#define VIDEOMESSAGEPARSER_HPP_

#include <iostream>
#include "../RtmpMessage.hpp"

namespace rtmp_protocol {

class MediaMsgParser {
 public:
  MediaMsgParser() {
  }
  virtual ~MediaMsgParser() {
  }
  bool parse_media_msg(RtmpHeader_ptr header, std::istream& stream, size_t buf_size,
             size_t& readed_size);
  RtmpMessage_ptr get_parsed_msg() {
    return parsed_msg_;
  }
  void set_parsed_msg(RtmpMessage_ptr msg);

  void set_connection_id(unsigned int id) {
    connection_id_ = id;
  }
 private:
  RtmpMessage_ptr parsed_msg_;
  unsigned int connection_id_;
};

}  // namespace rtmp_protocol

#endif /* VIDEOMESSAGEPARSER_HPP_ */
