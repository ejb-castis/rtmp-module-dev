#ifndef RTMPMESSAGE_HPP_
#define RTMPMESSAGE_HPP_

#include "../network/Message.hpp"
#include "../common/StringUtil.hpp"
#include "../common/StreamUtil.hpp"
#include "RtmpHeader.hpp"
#include <sstream>

namespace rtmp_protocol {

class PublishType {
  public:
  typedef enum {
    record,
    append,
    live,    
  } type;
};

class RtmpMessageType {
 public:
  typedef enum {
    UNKNOWN,
    SET_CHUNK_SIZE,
    ABORT,
    ACK,
    WIN_ACK_SIZE,
    PEER_BW,
    AUDIO,
    VIDEO,
    STREAM_BEGIN,
    STREAM_EOF,
    STREAM_DRY,
    SET_BUFFER_LEN,
    STREAM_IS_RECORDED,
    PING_REQUEST,
    PING_RESPONSE,
    CONNECT,
    CONNECT_RESULT,
    ON_BW_DONE,
    RELEASE_STREAM,
    FC_PUBLISH,
    FC_UNPUBLISH,
    CREATE_STREAM,
    DELETE_STREAM,
    CALL_ERROR,
    SIMPLE_RESULT,
    ON_FC_PUBLISH,
    PUBLISH,
    PLAY,
    ON_STATUS,
    RTMP_SAMPLE_ACCESS,
    ON_METADATA,
  } type;
};

class RtmpMessage : public rtmp_network::Message {
private:
  RtmpHeader_ptr header_ptr_;
  RtmpMessageType::type msg_type_;
  unsigned int write_chunk_size_;
  
public:
  RtmpMessage();
  RtmpMessage(RtmpHeader_ptr header);
  virtual ~RtmpMessage() {}

  virtual std::string get_class_name() {
    return "RtmpMessage";
  }
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "id[" << id_ <<"],";
    oss << "timestamp[" << header_ptr_->timestamp_ <<"],";
    oss << "type[" << header_ptr_->msg_type_id_ <<"],";
    oss << "length[" << header_ptr_->msg_length_ <<"],";
    oss << "header[" << get_header()->to_string() <<"]";
    return oss.str();
  }
  // return value : writed bytes count in stream
  virtual size_t write_payload(std::ostream& stream) {
    return 0;
  }
  RtmpMessageType::type get_type() {
    return msg_type_;
  }
  RtmpHeader_ptr get_header() {
    return header_ptr_;
  }

  RtmpHeaderFormat::format_type format_type_;
  unsigned int chunk_stream_id_;
  unsigned int timestamp_;
  unsigned int msg_length_;
  RtmpHeaderMsgTypeId::type msg_type_id_;
  unsigned int msg_stream_id_;
  unsigned int timestamp_delta_;

  long long abs_timestamp_;  // not used for network data
  std::size_t id_;

  unsigned int chunk_stream_id() {
    return header_ptr_->chunk_stream_id_;
  }
  unsigned int msg_stream_id() {
    return header_ptr_->msg_stream_id_;
  }
  
  void set_type(RtmpMessageType::type type) {
    msg_type_ = type;
  }
  void set_header(RtmpHeader_ptr header) {
    header_ptr_.reset();
    header_ptr_ = header;
  }
  void set_write_chunk_size(unsigned int size) {
    write_chunk_size_ = size;
  }
  void set_header_format_type(RtmpHeaderFormat::format_type type) {
    get_header()->format_type_ = type;
  }
  void set_msg_type_id(RtmpHeaderMsgTypeId::type id) {
    get_header()->msg_type_id_ = id;
  }
  void set_msg_length(unsigned int len) {
    get_header()->msg_length_ = len;
  }
  void set_chunk_stream_id(unsigned int id) {
    get_header()->chunk_stream_id_ = id;
  }
  void set_msg_stream_id(unsigned int id) {
    get_header()->msg_stream_id_ = id;
  }
  void set_timestamp(unsigned int val) {
    get_header()->timestamp_= val;
    get_header()->timestamp_delta_= val;
  }
  size_t write_payload_with_body(std::ostringstream& in, std::ostream& out) {
    std::string in_str = in.str();
    const size_t total_len = in_str.length();
    size_t len = total_len;
    size_t writed = total_len;

    bool divide_by_chunk = total_len > write_chunk_size_;

    for (int i = 0; len > write_chunk_size_; len -= write_chunk_size_, i++) {
      if (i != 0) {
        RtmpHeader header;
        header.format_type_ = RtmpHeaderFormat::CONTINUATION;
        header.chunk_stream_id_ = get_header()->chunk_stream_id_;

        size_t writed_header = header.write_payload(out);
        writed += writed_header;
      }
      out.write(&(in_str.c_str()[i * write_chunk_size_]), write_chunk_size_);
    }

    if (len > 0) {
      int index = (total_len - 1) / write_chunk_size_;

      if (divide_by_chunk) {
        RtmpHeader header;
        header.format_type_ = RtmpHeaderFormat::CONTINUATION;
        header.chunk_stream_id_ = get_header()->chunk_stream_id_;
        size_t writed_header = header.write_payload(out);
        writed += writed_header;
      }
      out.write(&(in_str.c_str()[index * write_chunk_size_]), len);
    }
    return writed;
  }
};
typedef boost::shared_ptr<RtmpMessage> RtmpMessage_ptr;

class UnknownMessage : public RtmpMessage {
 public:
  std::string buf_;

  void set_buf(std::string buf) {
    buf_ = buf;
  }
  std::string get_buf() {
    return buf_;
  }

  std::string to_string() {
    std::ostringstream oss;
    oss << "UnknownMessage:";
    oss << string_util::get_hex_format(buf_);
    return oss.str();
  }
};
typedef boost::shared_ptr<UnknownMessage> UnknownMessage_ptr;
}  // namespace rtmp_protocol

#endif /* RTMPMESSAGE_HPP_ */
