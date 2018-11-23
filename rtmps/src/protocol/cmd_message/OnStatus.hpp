#ifndef ONSTATUS_HPP_
#define ONSTATUS_HPP_

#include "CommandMessage.hpp"

#define ON_STATUS_STR_LEVEL "level"
#define ON_STATUS_STR_LEVEL_STATUS "status"
#define ON_STATUS_STR_CODE "code"
#define ON_STATUS_STR_DESCRIPTION "description"
#define ON_STATUS_STR_DETAILS "details"
#define ON_STATUS_STR_CLIENTID "clientid"

#define ON_STATUS_STR_CODE_PUBLISH_START "NetStream.Publish.Start"
#define ON_STATUS_STR_CODE_PLAY_RESET "NetStream.Play.Reset"
#define ON_STATUS_STR_CODE_PLAY_START "NetStream.Play.Start"

#define ON_STATUS_STR_DESCR_PUBLISH_START_BEGIN "Stream `"
#define ON_STATUS_STR_DESCR_PUBLISH_START_END "` is now published"
#define ON_STATUS_STR_DESCR_PLAY_RESET "reset..."
#define ON_STATUS_STR_DESCR_PLAY_START "start..."

namespace rtmp_protocol {

class OnStatusCode {
 public:
  typedef enum {
    ON_STATUS_CODE_PUBLISH_START,
    ON_STATUS_CODE_PLAY_START,
    ON_STATUS_CODE_PLAY_RESET
  } type;

  static std::string get_type_string(type code) {
    switch (code) {
      case ON_STATUS_CODE_PUBLISH_START:
        return ON_STATUS_STR_CODE_PUBLISH_START;
      case ON_STATUS_CODE_PLAY_START:
        return ON_STATUS_STR_CODE_PLAY_START;
      case ON_STATUS_CODE_PLAY_RESET:
        return ON_STATUS_STR_CODE_PLAY_RESET;
    }
  }

  static std::string get_description(type code, std::string& stream_name) {
    switch (code) {
      case ON_STATUS_CODE_PUBLISH_START: {
        std::string begin(ON_STATUS_STR_DESCR_PUBLISH_START_BEGIN);
        std::string end(ON_STATUS_STR_DESCR_PUBLISH_START_END);
        return begin + stream_name + end;
      }
      case ON_STATUS_CODE_PLAY_START: {
        return ON_STATUS_STR_DESCR_PLAY_START;
      }
      case ON_STATUS_CODE_PLAY_RESET: {
        return ON_STATUS_STR_DESCR_PLAY_RESET;
      }
    }
  }
};

class OnStatus : public CommandMessage {
 private:
  std::string stream_name_;
  std::string client_id_;
  OnStatusCode::type code_;

 public:
  OnStatus() {
    set_type(RtmpMessageType::ON_STATUS);
  }
  OnStatus(RtmpHeaderFormat::format_type header_type,
           unsigned int chunk_stream_id, unsigned int msg_stream_id,
           double transaction_id, unsigned int write_chunk_size,
           OnStatusCode::type code, std::string& stream_name,
           std::string& client_id)
      : code_(code),
        stream_name_(stream_name),
        client_id_(client_id) {
    set_header_format_type(header_type);
    set_chunk_stream_id(chunk_stream_id);
    set_msg_stream_id(msg_stream_id);
    set_transaction_id(transaction_id);
    set_type(RtmpMessageType::ON_STATUS);
    set_write_chunk_size(write_chunk_size);
  }
  virtual ~OnStatus() {
  }
  virtual std::string get_class_name() {
    return "OnStatus";
  }
  virtual size_t write_body_payload(std::ostream& stream);

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "OnStatus:";
    oss << CommandMessage::to_string();
    oss << ", code:" << OnStatusCode::get_type_string(code_);
    oss << ", stream_name:" << stream_name_;
    oss << ", client_id:" << client_id_;
    return oss.str();
  }
};
typedef boost::shared_ptr<OnStatus> OnStatus_ptr;
}  // namespace rtmp_protocol

#endif /* ONSTATUS_HPP_ */
