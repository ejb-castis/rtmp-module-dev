#ifndef FCPUBLISH_HPP_
#define FCPUBLISH_HPP_

#include "CommandMessage.hpp"

namespace rtmp_protocol {

class FCPublish : public CommandMessage {
 public:
  FCPublish() {
    set_type(RtmpMessageType::FC_PUBLISH);
  }
  virtual ~FCPublish() {
  }
  virtual std::string get_class_name() {
    return "FCPublish";
  }

  // not implemented
  virtual size_t write_body_payload(std::ostream& stream) {
    return 0;
  }

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "FCPublish:";
    oss << CommandMessage::to_string();
    oss << ", stream:" << stream_name_;
    return oss.str();
  }

  std::string stream_name_;
};
typedef boost::shared_ptr<FCPublish> FCPublish_ptr;


class FCUnPublish : public CommandMessage {
 public:
  FCUnPublish(double transaction_id, std::string stream_name) {
    set_type(RtmpMessageType::FC_UNPUBLISH);
    set_transaction_id(transaction_id);
    set_stream_name(stream_name);
  }
  virtual ~FCUnPublish() {
  }
  virtual std::string get_class_name() {
    return "FCUnPublish";
  }
  std::string stream_name() { return stream_name_; }
  void set_stream_name(std::string stream_name) { stream_name_ = stream_name; }

  // not implemented
  virtual size_t write_body_payload(std::ostream& stream) {
    return 0;
  }

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "FCUnPublish:";
    oss << CommandMessage::to_string();
    oss << ", stream:" << stream_name_;
    return oss.str();
  }

  std::string stream_name_;
};
typedef boost::shared_ptr<FCUnPublish> FCUnPublish_ptr;


}  // namespace rtmp_protocol

#endif /* FCPUBLISH_HPP_ */
