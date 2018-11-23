#include <sstream>
#include "RtmpHeader.hpp"
#include "../common/StreamUtil.hpp"
#include "../common/NetworkUtil.hpp"

namespace rtmp_protocol {
std::string RtmpHeader::to_string() {
  std::ostringstream oss;
  oss << "RtmpHeader:format_type:" << static_cast<int>(format_type_)
      << ",chunk_stream_id:" << chunk_stream_id_;

  if (format_type_ == RtmpHeaderFormat::FULL) {
    oss << ",timestamp:" << timestamp_;
    oss << ",msg_length:" << msg_length_;
    oss << ",msg_type_id:" << static_cast<int>(msg_type_id_);
    oss << ",msg_stream_id:" << msg_stream_id_;
  } else if (format_type_ == RtmpHeaderFormat::SAME_STREAM) {
    oss << ",timestamp_delta:" << timestamp_delta_;
    oss << ",msg_length:" << msg_length_;
    oss << ",msg_type_id:" << static_cast<int>(msg_type_id_);
    oss << ",msg_stream_id:" << msg_stream_id_;
  } else if (format_type_ == RtmpHeaderFormat::SAME_LENGTH_AND_STREAM) {
    oss << ",timestamp_delta:" << timestamp_delta_;
    oss << ",msg_length:" << msg_length_;
    oss << ",msg_type_id:" << static_cast<int>(msg_type_id_);
    oss << ",msg_stream_id:" << msg_stream_id_;
  }
  oss << ",abs_timestamp:" << abs_timestamp_;

  return oss.str();
}

size_t RtmpHeader::write_payload(std::ostream& stream) {
  size_t writed = 0;
  if (is_1byte_basic_header()) {
    unsigned char basic = format_type_ << 6;
    basic += chunk_stream_id_;
    stream_util::write_uchar(stream, basic, writed);
    //writed += 1;
  } else if (is_2byte_basic_header()) {
    unsigned char basic = format_type_ << 6;
    stream_util::write_uchar(stream, basic, writed);
    unsigned char id = static_cast<unsigned char>(chunk_stream_id_ - 64);
    stream_util::write_uchar(stream, id, writed);
    //writed += 2;
  } else if (is_3byte_basic_header()) {
    unsigned char basic = format_type_ << 6;
    basic += 1;
    stream_util::write_uchar(stream, basic, writed);
    unsigned int id_temp = chunk_stream_id_ - 64;
    unsigned int cs_id_high = static_cast<unsigned int>(id_temp / 256);
    unsigned int cs_id_low = static_cast<unsigned int>(id_temp % 256);
    stream_util::write_uchar(stream, cs_id_high, writed);
    stream_util::write_uchar(stream, cs_id_low, writed);
    //writed += 3;
  }

  switch (format_type_) {
    case RtmpHeaderFormat::FULL: {
      // TODO: extended timestamp 처리 필요.
      stream_util::write_uint_3(stream, timestamp_, true, writed);
      stream_util::write_uint_3(stream, msg_length_, true, writed);
      stream_util::write_uchar(stream, msg_type_id_, writed);
      stream_util::write_uint(stream, msg_stream_id_, false, writed);
      //writed += 11;
      break;
    }
    case RtmpHeaderFormat::SAME_STREAM: {
      stream_util::write_uint_3(stream, timestamp_delta_, true, writed);
      stream_util::write_uint_3(stream, msg_length_, true, writed);
      stream_util::write_uchar(stream, msg_type_id_, writed);
      //writed += 7;
      break;
    }
    case RtmpHeaderFormat::SAME_LENGTH_AND_STREAM: {
      stream_util::write_uint_3(stream, timestamp_delta_, true, writed);
      //writed += 3;
      break;
    }
    case RtmpHeaderFormat::CONTINUATION: {
      break;
    }
    default: {
      break;
    }
  }

  return writed;
}

}  // namespace rtmp_protocol
