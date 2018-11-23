#ifndef AUDIOMESSAGE_HPP_
#define AUDIOMESSAGE_HPP_

#include "../../common/DataType.hpp"
#include "../RtmpMessage.hpp"

namespace rtmp_protocol {
class MediaMessage : public RtmpMessage {
 private:
  uchar_array data_;
  size_t data_len_;

 public:
  MediaMessage()
      : data_len_(0) {
  }
  virtual ~MediaMessage() {
  }
  virtual std::string get_class_name() {
    return "MediaMessage";
  }

  virtual std::string to_string() {
    return "";
  }
  virtual size_t write_payload(std::ostream& stream) {
    std::ostringstream body_stream;
    body_stream.write(reinterpret_cast<const char*>(data_.get()), data_len_);

    get_header()->msg_length_ = data_len_;

    size_t writed_header = get_header()->write_payload(stream);
    size_t writed_body = RtmpMessage::write_payload_with_body(body_stream,
                                                              stream);

    return writed_header + writed_body;
  }
  void set_data(uchar_array& data, size_t len) {
    data_ = data;
    data_len_ = len;
  }
  uchar_array get_data() {
    return data_;
  }
  size_t get_data_len() {
    return data_len_;
  }
};
typedef boost::shared_ptr<MediaMessage> MediaMessage_ptr;

class AudioMessage : public MediaMessage {
 public:
  AudioMessage()
      : MediaMessage() {
    set_type(RtmpMessageType::AUDIO);
    set_msg_type_id(RtmpHeaderMsgTypeId::AUDIODATA);
  }

  AudioMessage(RtmpHeaderFormat::format_type header_type,
               unsigned int chunk_stream_id, unsigned int msg_stream_id,
               unsigned int write_chunk_size, unsigned int timestamp,
               uchar_array& data, size_t data_len)
      : MediaMessage() {
    set_type(RtmpMessageType::AUDIO);
    set_msg_type_id(RtmpHeaderMsgTypeId::AUDIODATA);
    set_header_format_type(header_type);
    set_write_chunk_size(write_chunk_size);
    set_chunk_stream_id(chunk_stream_id);
    set_msg_stream_id(msg_stream_id);
    set_timestamp(timestamp);
    set_data(data, data_len);
  }
  virtual ~AudioMessage() {
  }
  virtual std::string get_class_name() {
    return "AudioMessage";
  }

  virtual std::string to_string() {
    size_t print_len = (get_data_len() > 30) ? 30 : get_data_len();
    std::string str(reinterpret_cast<char*>(get_data().get()), print_len);
    std::ostringstream oss;
    oss << "AudioMessage:";
    oss << RtmpMessage::to_string();
    oss << ",data_len:" << get_data_len();
    oss << ",data:" << string_util::get_hex_format(str);
    return oss.str();
  }
};
typedef boost::shared_ptr<AudioMessage> AudioMessage_ptr;

class VideoMessage : public MediaMessage {
 public:
  VideoMessage()
      : MediaMessage() {
    set_type(RtmpMessageType::VIDEO);
    set_msg_type_id(RtmpHeaderMsgTypeId::VIDEODATA);
  }
  VideoMessage(RtmpHeaderFormat::format_type header_type,
                 unsigned int chunk_stream_id, unsigned int msg_stream_id,
                 unsigned int write_chunk_size, unsigned int timestamp,
                 uchar_array& data, size_t data_len)
        : MediaMessage() {
      set_type(RtmpMessageType::VIDEO);
      set_msg_type_id(RtmpHeaderMsgTypeId::VIDEODATA);
      set_header_format_type(header_type);
      set_write_chunk_size(write_chunk_size);
      set_chunk_stream_id(chunk_stream_id);
      set_msg_stream_id(msg_stream_id);
      set_timestamp(timestamp);
      set_data(data, data_len);
    }
  virtual ~VideoMessage() {
  }
  virtual std::string get_class_name() {
    return "VideoMessage";
  }

  virtual std::string to_string() {
    size_t print_len = (get_data_len() > 30) ? 30 : get_data_len();
    std::string str(reinterpret_cast<char*>(get_data().get()), print_len);
    std::ostringstream oss;
    oss << "VideoMessage:";
    oss << RtmpMessage::to_string();
    oss << ",data_len:" << get_data_len();
    oss << ",data:" << string_util::get_hex_format(str);

    return oss.str();
  }
};
typedef boost::shared_ptr<VideoMessage> VideoMessage_ptr;

}  // namespace rtmp_protocol

#endif /* AUDIOMESSAGE_HPP_ */
