#ifndef ONBWDONE_HPP_
#define ONBWDONE_HPP_

#include "CommandMessage.hpp"

namespace rtmp_protocol {

class OnBWDone : public CommandMessage {
 private:
  double speed_kbps_;

 public:
  OnBWDone() {
    set_type(RtmpMessageType::ON_BW_DONE);
  }
  OnBWDone(RtmpHeaderFormat::format_type header_type,
           unsigned int chunk_stream_id, double transaction_id,
           unsigned int write_chunk_size, double speed_kbps)
      : speed_kbps_(speed_kbps) {
    set_header_format_type(header_type);
    set_type(RtmpMessageType::ON_BW_DONE);
    set_write_chunk_size(write_chunk_size);
    set_chunk_stream_id(chunk_stream_id);
    set_transaction_id(transaction_id);
  }
  virtual ~OnBWDone() {
  }
  virtual std::string get_class_name() {
    return "OnBWDone";
  }
  virtual size_t write_body_payload(std::ostream& stream);

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "OnBWDone:";
    oss << CommandMessage::to_string();
    oss << ", speed_kbps:" << speed_kbps_;
    return oss.str();
  }
};
typedef boost::shared_ptr<OnBWDone> OnBWDone_ptr;
}  // namespace rtmp_protocol

#endif /* ONBWDONE_HPP_ */
