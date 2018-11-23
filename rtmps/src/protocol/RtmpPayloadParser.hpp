#ifndef RTMPPAYLOADPARSER_HPP_
#define RTMPPAYLOADPARSER_HPP_

#include <boost/logic/tribool.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <map>
#include <streambuf>

#include "RtmpHeader.hpp"
#include "control_message/ControlMsgParser.hpp"
#include "cmd_message/CommandMsgParser.hpp"
#include "media_message/MediaMsgParser.hpp"
#include "data_message/DataMsgParser.hpp"

#define MAX_CHANNEL_COUNT 319

namespace rtmp_protocol {

class RtmpPayloadParseResult {
 public:
  typedef enum {
    COMPLETE,
    FAIL,
    WAITING_MORE_PAYLOAD_DATA,
    WAITING_NEXT_CHUNK
  } type;
};

class RtmpPayloadParser {
 public:
  RtmpPayloadParser();
  virtual ~RtmpPayloadParser() {
  }
  RtmpPayloadParseResult::type parse_payload(RtmpHeader_ptr header_ptr,
                                     std::istream& stream, size_t buf_size,
                                     size_t& readed_size);

  RtmpMessage_ptr get_parsed_msg() {
    return parsed_msg_;
  }
  void set_recv_chunk_size(unsigned int size) {
    recv_chunk_size_ = size;
  }
  void set_connection_id(unsigned int id) {
    connection_id_ = id;
    control_msg_parser_.set_connection_id(id);
    command_msg_parser_.set_connection_id(id);
    media_msg_parser_.set_connection_id(id);
    data_msg_parser_.set_connection_id(id);
  }

 private:
  typedef struct received_payload_info {
    RtmpHeader_ptr first_chunk_header;
    unsigned int recv_length;
    unsigned char *recv_buf;
  } received_payload_info;

  typedef boost::shared_ptr<received_payload_info> received_payload_info_ptr;
  typedef std::map<unsigned int, received_payload_info_ptr> parsing_info_map;
  typedef std::pair<unsigned int, received_payload_info_ptr> parsing_info_pair;

  parsing_info_map incomplete_infos_;

  typedef struct channel_info {
    unsigned int last_msg_len;
    RtmpHeaderMsgTypeId::type last_msg_type_id;
    unsigned int last_msg_timestamp;
  } channel_info;

  channel_info channels_[MAX_CHANNEL_COUNT];

  ControlMsgParser control_msg_parser_;
  CommandMsgParser command_msg_parser_;
  MediaMsgParser media_msg_parser_;
  DataMsgParser data_msg_parser_;

  RtmpMessage_ptr parsed_msg_;

  unsigned int recv_chunk_size_;
  unsigned int connection_id_;

  bool parse_unknown_message(RtmpHeader_ptr header_ptr, std::istream& stream,
                             size_t buf_size);
  void set_parsed_msg(RtmpMessage_ptr msg);
};

}  // namespace rtmp_protocol

#endif /* RTMPPAYLOADPARSER_HPP_ */
