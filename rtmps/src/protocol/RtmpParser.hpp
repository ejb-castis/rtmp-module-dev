#ifndef RTMPPARSER_HPP_
#define RTMPPARSER_HPP_

#include "../network/RequestParser.hpp"
#include "RtmpHeaderParser.hpp"
#include "RtmpPayloadParser.hpp"

#define DEFAULT_WIN_ACK_SIZE 131072

namespace rtmp_protocol {

class RtmpParser : public rtmp_network::RequestParser {
 private:
  typedef enum {
    HEADER_PARSING,
    PAYLOAD_PARSING,
    COMPLETE
  } parsing_state;

  parsing_state parsing_state_;
  unsigned int connection_id_;
  unsigned long long readed_bytes_count_;
  unsigned int win_ack_size_;
  unsigned int next_ack_send_size_;

  RtmpHeaderParser header_parser_;
  RtmpPayloadParser payload_parser_;

  RtmpHeader_ptr parsed_header_;
  RtmpMessage_ptr parsed_msg_;

  void set_abs_timestamp(RtmpHeader_ptr header);
  void set_parsed_msg(RtmpMessage_ptr msg);
  void set_parsed_header(RtmpHeader_ptr header);
  boost::tribool parse_private(std::istream& stream, size_t buf_size,
                                   size_t& total_readed_size);
  void set_win_ack_size(unsigned int size);

 public:
  RtmpParser();
  RtmpParser(rtmp_network::MessageSender_wptr sender, unsigned int connection_id);
  virtual ~RtmpParser() {
  }
  boost::tribool parse(std::istream& stream, size_t buf_size,
                       size_t& readed_size);

  rtmp_network::Message_ptr get_parsed_message();
  void set_recv_chunk_size(unsigned int size) {
    payload_parser_.set_recv_chunk_size(size);
  }
  void reset();
};
typedef boost::shared_ptr<RtmpParser> RtmpParser_ptr;

class RtmpParserFactory : public rtmp_network::RequestParserFactory {
 public:
  RtmpParserFactory() {
  }
  virtual ~RtmpParserFactory() {
  }
  virtual rtmp_network::RequestParser_ptr get_request_parser(
      rtmp_network::MessageSender_wptr sender, unsigned int id) {
    return RtmpParser_ptr(new RtmpParser(sender, id));
  }
};
typedef boost::shared_ptr<RtmpParserFactory> RtmpParserFactory_ptr;
}  // namespace rtmp_protocol
#endif /* RTMPPARSER_HPP_ */
