#ifndef PUBLISH_HPP_
#define PUBLISH_HPP_

#include "CommandMessage.hpp"

namespace rtmp_protocol {

class Publish : public CommandMessage {
 public:
  Publish() {
    set_type(RtmpMessageType::PUBLISH);
  }
  virtual ~Publish() {
  }
  virtual std::string get_class_name() {
    return "Publish";
  }
  // not implemented
  virtual size_t write_body_payload(std::ostream& stream) {
    return 0;
  }

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "Publish:";
    oss << CommandMessage::to_string();
    oss << ", stream name:" << stream_name_;
    oss << ", stream type:" << stream_type_;
    return oss.str();
  }

  std::string stream_name_;
  std::string stream_type_;
};
typedef boost::shared_ptr<Publish> Publish_ptr;
}  // namespace rtmp_protocol

#endif /* PUBLISH_HPP_ */
