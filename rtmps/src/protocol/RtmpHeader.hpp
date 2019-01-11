#ifndef RTMPHEADER_HPP_
#define RTMPHEADER_HPP_

#include <boost/shared_ptr.hpp>
#include <string>

namespace rtmp_protocol {

class RtmpHeaderFormat {
 public:
  typedef enum {
    FULL = 0,
    SAME_STREAM = 1,
    SAME_LENGTH_AND_STREAM = 2,
    CONTINUATION = 3,
    UNKNOWN = 0xFF
  } format_type;

  static format_type get_type(unsigned char value) {
    if (value != 0 && value != 1 && value != 2 && value != 3)
      return UNKNOWN;
    return static_cast<format_type>(value);
  }
};

class RtmpHeaderMsgTypeId {
 public:
  typedef enum {
    CHUNKSIZE = 1,
    ABORTMESSAGE = 2,
    ACK = 3,
    USRCTRL = 4,
    WINACKSIZE = 5,
    PEERBANDWIDTH = 6,
    AUDIODATA = 8,
    VIDEODATA = 9,
    DATA_AMF3 = 15,
    COMMAND_AMF3 = 17,
    DATA_AMF0 = 18,
    COMMAND_AMF0 = 20,
    UNKNOWN = 0xFF
  } type;

  static type get_id(unsigned char value) {
    if (value != 1 && value != 2 && value != 3 && value != 4 && value != 5
        && value != 6 && value != 8 && value != 9 && value != 15 && value != 17
        && value != 18 && value != 20)
      return UNKNOWN;
    return static_cast<type>(value);
  }
};

class RtmpHeader {
 private:
  bool is_1byte_basic_header() {
    return chunk_stream_id_ >= 2 && chunk_stream_id_ <= 63;
  }
  bool is_2byte_basic_header() {
    return chunk_stream_id_ >= 64 && chunk_stream_id_ <= 319;
  }
  bool is_3byte_basic_header() {
    return chunk_stream_id_ > 319 && chunk_stream_id_ <= 6559;
  }

 public:
  RtmpHeaderFormat::format_type format_type_;
  unsigned int chunk_stream_id_;
  unsigned int timestamp_;
  unsigned int msg_length_;
  RtmpHeaderMsgTypeId::type msg_type_id_;
  unsigned int msg_stream_id_;
  unsigned int timestamp_delta_;

  uint64_t abs_timestamp_;  // 64 bit value for 32 bit timestamp with rollover
  std::size_t message_id_{0};

  RtmpHeader() {
    reset();
  }
  RtmpHeader(const RtmpHeader* other) {
    format_type_ = other->format_type_;
    chunk_stream_id_ = other->chunk_stream_id_;
    timestamp_ = other->timestamp_;
    msg_length_ = other->msg_length_;
    msg_type_id_ = other->msg_type_id_;
    msg_stream_id_ = other->msg_stream_id_;
    timestamp_delta_ = other->timestamp_delta_;
    abs_timestamp_ = other->abs_timestamp_;
    message_id_ = other->message_id_;
  }
  virtual ~RtmpHeader() {
  }

  void reset() {
    format_type_ = RtmpHeaderFormat::FULL;
    chunk_stream_id_ = 0;
    timestamp_ = 0;
    msg_length_ = 0;
    msg_type_id_ = RtmpHeaderMsgTypeId::UNKNOWN;
    msg_stream_id_ = 0;
    timestamp_delta_ = 0;
    abs_timestamp_ = 0;
    message_id_=0;
  }

  bool has_msg_len_and_type() {
    return (format_type_ == RtmpHeaderFormat::FULL)
        || (format_type_ == RtmpHeaderFormat::SAME_STREAM);
  }

  bool is_first_stream_chunk() {
    return (format_type_ == RtmpHeaderFormat::FULL);
  }

  bool is_control_msg() {
    return msg_type_id_ >= 1 && msg_type_id_ <= 7;
  }

  bool is_command_msg() {
    return msg_type_id_ == RtmpHeaderMsgTypeId::COMMAND_AMF0
        || msg_type_id_ == RtmpHeaderMsgTypeId::COMMAND_AMF3;
  }

  bool is_data_msg() {
    return msg_type_id_ == RtmpHeaderMsgTypeId::DATA_AMF0
        || msg_type_id_ == RtmpHeaderMsgTypeId::DATA_AMF3;
  }

  bool is_video_msg() {
    return msg_type_id_ == RtmpHeaderMsgTypeId::VIDEODATA;
  }

  bool is_audio_msg() {
    return msg_type_id_ == RtmpHeaderMsgTypeId::AUDIODATA;
  }

  int get_msg_length_pos() {
    if (!has_msg_len_and_type())
      return -1;

    int len = 0;
    if (is_1byte_basic_header())
      len = 1;
    else if (is_2byte_basic_header())
      len = 2;
    else if (is_3byte_basic_header())
      len = 3;
    return len + 3;
  }
  std::string to_string();
  size_t write_payload(std::ostream& stream);
};
typedef boost::shared_ptr<RtmpHeader> RtmpHeader_ptr;
}  // namespace rtmp_protocol
#endif /* RTMPHEADER_HPP_ */
