#ifndef COMMANDMESSAGEPARSER_HPP_
#define COMMANDMESSAGEPARSER_HPP_

#include <iostream>
#include "CommandMessage.hpp"
#include "../RtmpHeader.hpp"
#include "../AMF0Serializer.hpp"

namespace rtmp_protocol {

class CommandMsgParser {
 public:
  CommandMsgParser() {
  }
  virtual ~CommandMsgParser() {
  }
  bool parse_command_msg(RtmpHeader_ptr header, std::istream& stream, size_t buf_size,
             size_t& readed_size);
  RtmpMessage_ptr get_parsed_msg() {
    return parsed_msg_;
  }
  void set_connection_id(unsigned int id) {
    connection_id_ = id;
  }
 private:
  bool parse_connect_msg(std::istream& stream, double transaction_id);
  bool parse_release_stream(std::istream& stream, double transaction_id);
  bool parse_fc_publish(std::istream& stream, double transaction_id);
  bool parse_fc_unpublish(std::istream& stream, double transaction_id);
  bool parse_create_stream(std::istream& stream, double transaction_id);
  bool parse_delete_stream(std::istream& stream, double transaction_id);
  bool parse_publish(std::istream& stream, double transaction_id);
  bool parse_unknown_command(std::istream& stream, double transaction_id,
                             std::string& command);
  bool parse_play(std::istream& stream, double transaction_id);

  void set_parsed_msg(RtmpMessage_ptr msg);
  RtmpMessage_ptr parsed_msg_;
  AMF0Serializer amf0_;
  unsigned int connection_id_;
};

}  // namespace rtmp_protocol

#endif /* COMMANDMESSAGEPARSER_HPP_ */
