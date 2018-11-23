#ifndef CONTROLMESSAGEPARSER_HPP_
#define CONTROLMESSAGEPARSER_HPP_

#include <iostream>
#include "ControlMessage.hpp"
#include "../RtmpHeader.hpp"

namespace rtmp_protocol {

class ControlMsgParser {
 public:
  ControlMsgParser() {
  }
  virtual ~ControlMsgParser() {
  }
  bool parse_control_msg(RtmpHeader_ptr header_ptr, std::istream& stream, size_t buf_size,
             size_t& readed_size);

  RtmpMessage_ptr get_parsed_msg() {
    return parsed_msg_;
  }
  void set_connection_id(unsigned int id) {
    connection_id_ = id;
  }

 private:
  RtmpMessage_ptr parsed_msg_;
  unsigned int connection_id_;

  bool parse_chunk_size(std::istream& stream);
  bool parse_abort_message(std::istream& stream);
  bool parse_ack(std::istream& stream);
  bool parse_user_ctrl(std::istream& stream);
  bool parse_win_ack_size(std::istream& stream);
  bool parse_peer_bandwidth(std::istream& stream);

  bool parse_usrctrl_stream_begin(std::istream& stream);
  bool parse_usrctrl_stream_eof(std::istream& stream);
  bool parse_usrctrl_stream_dry(std::istream& stream);
  bool parse_usrctrl_set_buffer_length(std::istream& stream);
  bool parse_usrctrl_stream_is_recorded(std::istream& stream);
  bool parse_usrctrl_ping_request(std::istream& stream);
  bool parse_usrctrl_ping_response(std::istream& stream);

  void set_parsed_msg(RtmpMessage_ptr msg) {
    parsed_msg_.reset();
    parsed_msg_ = msg;
  }
};

}  // namespace rtmp_protocol

#endif /* CONTROLMESSAGEPARSER_HPP_ */
