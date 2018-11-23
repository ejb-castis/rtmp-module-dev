#ifndef ONFCPUBLISH_HPP_
#define ONFCPUBLISH_HPP_

#include "CommandMessage.hpp"

#define ON_FC_PUBLISH_STR_CODE "code"
#define ON_FC_PUBLISH_STR_START "NetStream.Publish.Start"
#define ON_FC_PUBLISH_STR_DESCRIPTION "description"

namespace rtmp_protocol {

class OnFCPublish : public CommandMessage {
 private:
  std::string stream_name_;

 public:
  OnFCPublish() {
    set_type(RtmpMessageType::ON_FC_PUBLISH);
  }
  OnFCPublish(RtmpHeaderFormat::format_type header_type,
              unsigned int chunk_stream_id, unsigned int msg_stream_id,
              double transaction_id, unsigned int write_chunk_size,
              std::string& stream_name)
      : stream_name_(stream_name) {
    set_header_format_type(header_type);
    set_chunk_stream_id(chunk_stream_id);
    set_msg_stream_id(msg_stream_id);
    set_transaction_id(transaction_id);
    set_type(RtmpMessageType::ON_FC_PUBLISH);
    set_write_chunk_size(write_chunk_size);
  }
  virtual ~OnFCPublish() {
  }
  virtual std::string get_class_name() {
    return "OnFCPublish";
  }
  virtual size_t write_body_payload(std::ostream& stream);

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "OnFCPublish:";
    oss << CommandMessage::to_string();
    oss << ", stream_name:" << stream_name_;
    return oss.str();
  }
};
typedef boost::shared_ptr<OnFCPublish> OnFCPublish_ptr;
}  // namespace rtmp_protocol

#endif /* ONFCPUBLISH_HPP_ */
