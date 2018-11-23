#ifndef RELEASESTREAM_HPP_
#define RELEASESTREAM_HPP_

#include "CommandMessage.hpp"

namespace rtmp_protocol {

class ReleaseStream : public CommandMessage {
 public:
  ReleaseStream() {
    set_type(RtmpMessageType::RELEASE_STREAM);
  }
  virtual ~ReleaseStream() {
  }
  virtual std::string get_class_name() {
    return "ReleaseStream";
  }
  // not implemented
  virtual size_t write_body_payload(std::ostream& stream) {
    return 0;
  }

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "ReleaseStream:";
    oss << CommandMessage::to_string();
    oss << ", stream:" << stream_name_;
    return oss.str();
  }

  std::string stream_name_;
};
typedef boost::shared_ptr<ReleaseStream> ReleaseStream_ptr;
}  // namespace rtmp_protocol

#endif /* RELEASESTREAM_HPP_ */
