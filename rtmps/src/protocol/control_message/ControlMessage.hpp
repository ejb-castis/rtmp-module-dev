#ifndef CONTROLMESSAGE_HPP_
#define CONTROLMESSAGE_HPP_

#include "../RtmpMessage.hpp"

namespace rtmp_protocol {

class ControlMessage : public RtmpMessage {
 public:
  ControlMessage() {
    RtmpHeader_ptr header = get_header();
    header->format_type_ = RtmpHeaderFormat::FULL;
    header->chunk_stream_id_ = 2;
    header->msg_stream_id_ = 0;
    header->timestamp_ = 0;
  }
  virtual ~ControlMessage() {
  }
  virtual std::string get_class_name() {
    return "ControlMessage";
  }
  virtual std::string to_string() {
    return "ControlMessage";
  }
  // return value : writed bytes count in stream
  virtual size_t write_payload(std::ostream& stream) {
    return 0;
  }
};
typedef boost::shared_ptr<ControlMessage> ControlMessage_ptr;

class SetChunkSize : public ControlMessage {
 public:
  explicit SetChunkSize(unsigned int chunk_size)
      : chunk_size_(chunk_size) {
    set_type(RtmpMessageType::SET_CHUNK_SIZE);
    set_msg_type_id(RtmpHeaderMsgTypeId::CHUNKSIZE);
    set_msg_length(4);
  }
  virtual ~SetChunkSize() {
  }
  virtual std::string get_class_name() {
    return "SetChunkSize";
  }
  unsigned int chunk_size_;

  virtual size_t write_payload(std::ostream& stream);

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "SetChunkSize:chunk_size:" << static_cast<unsigned int>(chunk_size_);
    return oss.str();
  }
};
typedef boost::shared_ptr<SetChunkSize> SetChunkSize_ptr;

class AbortMessage : public ControlMessage {
 public:
  explicit AbortMessage(unsigned int chunk_stream_id)
      : chunk_stream_id_(chunk_stream_id) {
    set_type(RtmpMessageType::ABORT);
    set_msg_type_id(RtmpHeaderMsgTypeId::ABORTMESSAGE);
    set_msg_length(4);
  }
  virtual ~AbortMessage() {
  }
  virtual std::string get_class_name() {
    return "AbortMessage";
  }
  virtual size_t write_payload(std::ostream& stream);

  unsigned int chunk_stream_id_;

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "AbortMessage:chunk_stream_id:" << chunk_stream_id_;
    return oss.str();
  }
};
typedef boost::shared_ptr<AbortMessage> AbortMessage_ptr;

class Acknowledgement : public ControlMessage {
 public:
  explicit Acknowledgement(unsigned int seq_num)
      : seq_num_(seq_num) {
    set_type(RtmpMessageType::ACK);
    set_msg_type_id(RtmpHeaderMsgTypeId::ACK);
    set_msg_length(4);
  }
  virtual ~Acknowledgement() {
  }
  virtual std::string get_class_name() {
    return "Acknowledgement";
  }
  virtual size_t write_payload(std::ostream& stream);

  unsigned int seq_num_;

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "Acknowledgement:seq_num:" << seq_num_;
    return oss.str();
  }
};
typedef boost::shared_ptr<Acknowledgement> Acknowledgement_ptr;

class UserControlMessage : public ControlMessage {
 public:
  typedef enum {
    EVENT_TYPE_STREAM_BEGIN = 0,
    EVENT_TYPE_STREAM_EOF = 1,
    EVENT_TYPE_STREAM_DRY = 2,
    EVENT_TYPE_SET_BUFFER_LENGTH = 3,
    EVENT_TYPE_STREAM_IS_RECORDED = 4,
    EVENT_TYPE_PING_REQUEST = 6,
    EVENT_TYPE_PING_RESPONSE = 7,
    EVENT_TYPE_UNKNOWN = 0xFF
  } event_type;

  event_type event_type_;

  UserControlMessage() {
    set_msg_type_id(RtmpHeaderMsgTypeId::USRCTRL);
  }
  UserControlMessage(event_type type) {
    set_msg_type_id(RtmpHeaderMsgTypeId::USRCTRL);
    event_type_ = type;
  }
  virtual ~UserControlMessage() {
  }
  virtual std::string get_class_name() {
    return "UserControlMessage";
  }
  virtual size_t write_payload(std::ostream& stream) {
    return 0;
  }
  void set_event_type(event_type type) {
    event_type_ = type;
  }

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "UserControlMessage:event_type:" << event_type_;
    return oss.str();
  }
  static event_type get_event_type(unsigned short type) {
    if (type < 8 && type != 5)
      return static_cast<event_type>(type);
    return EVENT_TYPE_UNKNOWN;
  }
};
typedef boost::shared_ptr<UserControlMessage> UserControlMessage_ptr;

class WindowAcknowledgementSize : public ControlMessage {
 public:
  explicit WindowAcknowledgementSize(unsigned int window_size)
      : window_size_(window_size) {
    set_type(RtmpMessageType::WIN_ACK_SIZE);
    set_msg_type_id(RtmpHeaderMsgTypeId::WINACKSIZE);
    set_msg_length(4);
  }
  virtual ~WindowAcknowledgementSize() {
  }
  virtual std::string get_class_name() {
    return "WindowAcknowledgementSize";
  }
  virtual size_t write_payload(std::ostream& stream);
  unsigned int window_size_;

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "WindowAcknowledgementSize:window_size:" << window_size_;
    return oss.str();
  }
};
typedef boost::shared_ptr<WindowAcknowledgementSize> WindowAcknowledgementSize_ptr;

class SetPeerBandwidth : public ControlMessage {
 public:
  typedef enum {
    LIMIT_TYPE_HARD = 0,
    LIMIT_TYPE_SOFT = 1,
    LIMIT_TYPE_DYNAMIC = 2,
    LIMIT_TYPE_UNKNOWN = 0xFF
  } limit_type;

  unsigned int window_size_;
  limit_type limit_type_;

  explicit SetPeerBandwidth(unsigned int window_size, limit_type type)
      : window_size_(window_size),
        limit_type_(type) {
    set_type(RtmpMessageType::PEER_BW);
    set_msg_type_id(RtmpHeaderMsgTypeId::PEERBANDWIDTH);
    set_msg_length(5);
  }
  virtual ~SetPeerBandwidth() {
  }
  virtual std::string get_class_name() {
    return "SetPeerBandwidth";
  }
  virtual size_t write_payload(std::ostream& stream);
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "SetPeerBandwidth:window_size:" << window_size_;
    oss << ",limit_type:" << limit_type_;
    return oss.str();
  }

  static limit_type get_limit_type(unsigned char type) {
    if (type < 3)
      return static_cast<limit_type>(type);
    return LIMIT_TYPE_UNKNOWN;
  }
};
typedef boost::shared_ptr<SetPeerBandwidth> SetPeerBandwidth_ptr;

/****************************
 *  children of UserControlMessage
 */
class StreamBegin : public UserControlMessage {
 public:
  explicit StreamBegin(unsigned int stream_id)
      : UserControlMessage(EVENT_TYPE_STREAM_BEGIN),
        stream_id_(stream_id) {
    set_type(RtmpMessageType::STREAM_BEGIN);
    set_msg_length(6);
  }
  virtual ~StreamBegin() {
  }
  virtual std::string get_class_name() {
    return "StreamBegin";
  }
  virtual size_t write_payload(std::ostream& stream);
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "StreamBegin:stream_id:" << stream_id_;
    return oss.str();
  }
  unsigned int stream_id_;
};
typedef boost::shared_ptr<StreamBegin> StreamBegin_ptr;

class StreamEof : public UserControlMessage {
 public:
  explicit StreamEof(unsigned int stream_id)
      : UserControlMessage(EVENT_TYPE_STREAM_EOF),
        stream_id_(stream_id) {
    set_type(RtmpMessageType::STREAM_EOF);
    set_msg_length(6);
  }
  virtual ~StreamEof() {
  }
  virtual std::string get_class_name() {
    return "StreamEof";
  }
  virtual size_t write_payload(std::ostream& stream);
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "StreamEof:stream_id:" << stream_id_;
    return oss.str();
  }
  unsigned int stream_id_;
};
typedef boost::shared_ptr<StreamEof> StreamEof_ptr;

class StreamDry : public UserControlMessage {
 public:
  explicit StreamDry(unsigned int stream_id)
      : UserControlMessage(EVENT_TYPE_STREAM_DRY),
        stream_id_(stream_id) {
    set_type(RtmpMessageType::STREAM_DRY);
    set_msg_length(6);
  }
  virtual ~StreamDry() {
  }
  virtual std::string get_class_name() {
    return "StreamDry";
  }
  virtual size_t write_payload(std::ostream& stream);
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "StreamDry:stream_id:" << stream_id_;
    return oss.str();
  }
  unsigned int stream_id_;
};
typedef boost::shared_ptr<StreamDry> StreamDry_ptr;

class SetBufferLength : public UserControlMessage {
 public:
  explicit SetBufferLength(unsigned int stream_id, unsigned int buffer_length)
      : UserControlMessage(EVENT_TYPE_SET_BUFFER_LENGTH),
        stream_id_(stream_id),
        buffer_length_(buffer_length) {
    set_type(RtmpMessageType::SET_BUFFER_LEN);
    set_msg_length(10);
  }
  virtual ~SetBufferLength() {
  }
  virtual std::string get_class_name() {
    return "SetBufferLength";
  }
  virtual size_t write_payload(std::ostream& stream);
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "SetBufferLength:stream_id:" << stream_id_ << ",buffer_length:"
        << buffer_length_;
    return oss.str();
  }
  unsigned int stream_id_;
  unsigned int buffer_length_;
};
typedef boost::shared_ptr<SetBufferLength> SetBufferLength_ptr;

class StreamIsRecorded : public UserControlMessage {
 public:
  explicit StreamIsRecorded(unsigned int stream_id)
      : UserControlMessage(EVENT_TYPE_STREAM_IS_RECORDED),
        stream_id_(stream_id) {
    set_type(RtmpMessageType::STREAM_IS_RECORDED);
    set_msg_length(6);
  }
  virtual ~StreamIsRecorded() {
  }
  virtual std::string get_class_name() {
      return "StreamIsRecorded";
    }
  virtual size_t write_payload(std::ostream& stream);
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "StreamIsRecorded:stream_id:" << stream_id_;
    return oss.str();
  }
  unsigned int stream_id_;
};
typedef boost::shared_ptr<StreamIsRecorded> StreamIsRecorded_ptr;

class PingRequest : public UserControlMessage {
 public:
  explicit PingRequest(unsigned int timestamp)
      : UserControlMessage(EVENT_TYPE_PING_REQUEST),
        timestamp_(timestamp) {
    set_type(RtmpMessageType::PING_REQUEST);
    set_msg_length(6);
  }
  virtual ~PingRequest() {
  }
  virtual std::string get_class_name() {
        return "PingRequest";
      }
  virtual size_t write_payload(std::ostream& stream);
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "PingRequest:timestamp:" << timestamp_;
    return oss.str();
  }
  unsigned int timestamp_;
};
typedef boost::shared_ptr<PingRequest> PingRequest_ptr;

class PingResponse : public UserControlMessage {
 public:
  PingResponse(unsigned int timestamp)
      : UserControlMessage(EVENT_TYPE_PING_RESPONSE),
        timestamp_(timestamp) {
    set_type(RtmpMessageType::PING_RESPONSE);
    set_msg_length(6);
  }
  virtual ~PingResponse() {
  }
  virtual std::string get_class_name() {
    return "PingResponse";
  }
  virtual size_t write_payload(std::ostream& stream);
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "PingResponse:timestamp:" << timestamp_;
    return oss.str();
  }
  unsigned int timestamp_;
};
typedef boost::shared_ptr<PingResponse> PingResponse_ptr;

}  // namespace rtmp_protocol

#endif /* CONTROLMESSAGE_HPP_ */
