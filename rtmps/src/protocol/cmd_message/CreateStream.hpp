#ifndef CREATESTREAM_HPP_
#define CREATESTREAM_HPP_

#include "CommandMessage.hpp"

namespace rtmp_protocol {

class CreateStream : public CommandMessage {
 public:
  CreateStream() {
    set_type(RtmpMessageType::CREATE_STREAM);
  }
  virtual ~CreateStream() {
  }
  virtual std::string get_class_name() {
    return "CreateStream";
  }
  // not implemented
  virtual size_t write_body_payload(std::ostream& stream) {
    return 0;
  }

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "CreateStream:";
    oss << CommandMessage::to_string();
    return oss.str();
  }
};
typedef boost::shared_ptr<CreateStream> CreateStream_ptr;


class DeleteStream : public CommandMessage {
 public:
  DeleteStream(double transaction_id, double stream_id) {
    set_type(RtmpMessageType::DELETE_STREAM);
    set_transaction_id(transaction_id);
    set_stream_id(stream_id);
  }
  virtual ~DeleteStream() {
  }

  double stream_id_;

  double stream_id() { return stream_id_; }
  void set_stream_id(double stream_id) { 
    stream_id_ = stream_id;
  }
  virtual std::string get_class_name() {
    return "DeleteStream";
  }
  // not implemented
  virtual size_t write_body_payload(std::ostream& stream) {
    return 0;
  }

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "DeleteStream:";
    oss << CommandMessage::to_string();
    oss << ", stream_id: " << stream_id_;
    return oss.str();
  }
};
typedef boost::shared_ptr<DeleteStream> DeleteStream_ptr;



}  // namespace rtmp_protocol



#endif /* CREATESTREAM_HPP_ */
