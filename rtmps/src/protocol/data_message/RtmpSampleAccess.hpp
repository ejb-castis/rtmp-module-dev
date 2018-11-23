#ifndef RTMPSAMPLEACCESS_HPP_
#define RTMPSAMPLEACCESS_HPP_

#include "DataMessage.hpp"

namespace rtmp_protocol {

class RtmpSampleAccess : public DataMessage {
 public:
  RtmpSampleAccess() {
    set_type(RtmpMessageType::RTMP_SAMPLE_ACCESS);
  }
  RtmpSampleAccess(RtmpHeaderFormat::format_type header_type,
                   unsigned int chunk_stream_id, unsigned int msg_stream_id,
                   unsigned int write_chunk_size) {
    set_header_format_type(header_type);
    set_type(RtmpMessageType::RTMP_SAMPLE_ACCESS);
    set_write_chunk_size(write_chunk_size);
    set_chunk_stream_id(chunk_stream_id);
    set_msg_stream_id(msg_stream_id);
  }
  virtual ~RtmpSampleAccess() {
  }
  virtual std::string get_class_name() {
    return "RtmpSampleAccess";
  }
  virtual size_t write_body_payload(std::ostream& stream);

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "RtmpSampleAccess:";
    oss << DataMessage::to_string();
    return oss.str();
  }
};
typedef boost::shared_ptr<RtmpSampleAccess> RtmpSampleAccess_ptr;
}  // namespace rtmp_protocol

#endif /* RTMPSAMPLEACCESS_HPP_ */
