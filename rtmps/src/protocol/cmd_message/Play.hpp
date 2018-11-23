#ifndef PLAY_HPP_
#define PLAY_HPP_

#include "CommandMessage.hpp"

namespace rtmp_protocol {

class Play : public CommandMessage {
 public:
  Play() {
    set_type(RtmpMessageType::PLAY);
  }
  virtual ~Play() {
  }
  virtual std::string get_class_name() {
    return "Play";
  }
  // not implemented
  virtual size_t write_body_payload(std::ostream& stream) {
    return 0;
  }

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "Play:";
    oss << CommandMessage::to_string();
    oss << ", stream:" << stream_name_;
    return oss.str();
  }

  std::string stream_name_;
};
typedef boost::shared_ptr<Play> Play_ptr;
}  // namespace rtmp_protocol

#endif /* PLAY_HPP_ */
