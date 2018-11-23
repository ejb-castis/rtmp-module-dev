#include "RtmpHeaderParser.hpp"
#include "../common/NetworkUtil.hpp"
#include "../../../src/rtmpmodulelogger.h"

namespace rtmp_protocol {
RtmpHeaderParser::RtmpHeaderParser() {
  reset();
}

RtmpHeaderParser::parsing_state RtmpHeaderParser::get_chunk_header_parsing_state(
    int header_format) {
  switch (header_format) {
    case 0:
      return CHUNK_MSG_HEADER_TYPE0;
    case 1:
      return CHUNK_MSG_HEADER_TYPE1;
    case 2:
      return CHUNK_MSG_HEADER_TYPE2;
    case 3:
      return CHUNK_MSG_HEADER_TYPE3;
  }
  return CHUNK_MSG_HEADER_TYPE0;
}

boost::tribool RtmpHeaderParser::parse_in_current_state(std::istream& stream,
                                                        size_t buf_size,
                                                        size_t& readed_size,
                                                        parsing_state state) {
  boost::tribool result = false;
  switch (state) {
    case BASIC_HEADER_1BYTE: {
      result = parse_basic_header_1byte(stream, buf_size, readed_size);
      break;
    }
    case BASIC_HEADER_2BYTE: {
      result = parse_basic_header_2byte(stream, buf_size, readed_size);
      break;
    }
    case BASIC_HEADER_3BYTE: {
      result = parse_basic_header_3byte(stream, buf_size, readed_size);
      break;
    }
    case CHUNK_MSG_HEADER_TYPE0: {
      result = parse_chunk_header_type0(stream, buf_size, readed_size);
      break;
    }
    case CHUNK_MSG_HEADER_TYPE1: {
      result = parse_chunk_header_type1(stream, buf_size, readed_size);
      break;
    }
    case CHUNK_MSG_HEADER_TYPE2: {
      result = parse_chunk_header_type2(stream, buf_size, readed_size);
      break;
    }
    case CHUNK_MSG_HEADER_TYPE3: {
      result = parse_chunk_header_type3(stream, buf_size, readed_size);
      break;
    }
    case EXTENDED_TIMESTAMP: {
      result = parse_extended_timestamp(stream, buf_size, readed_size);
      break;
    }
    case COMPLETE: {
      result = true;
      break;
    }
  }

  return result;
}

void RtmpHeaderParser::reset() {
  parsed_msg_.reset(new RtmpHeader);
  parsing_state_ = BASIC_HEADER_1BYTE;
}

boost::tribool RtmpHeaderParser::parse_header(std::istream& stream, size_t buf_size,
                                       size_t& total_readed_size) {
  total_readed_size = 0;
  while (true) {

    if (parsing_state_ == COMPLETE) {
      parsing_state_ = BASIC_HEADER_1BYTE;
      return true;
    }

    size_t readed_size = 0;
    boost::tribool result = parse_in_current_state(stream, buf_size,
                                                   readed_size, parsing_state_);
    if (result) {
      buf_size -= readed_size;
      total_readed_size += readed_size;
    } else {
      RTMPLOGF(debug,"parse_in_current_state[false],parsing_state[%1%],buf_size[%2%],read_size[%3%]",parsing_state_ ,buf_size, readed_size);
      return result;
    }
  }

  RTMPLOGF(debug,"parse_in_current_state[false],parsing_state[%1%],buf_size[%2%],total_read_size[%3%]",parsing_state_ ,buf_size, total_readed_size);
  return false;
}

boost::tribool RtmpHeaderParser::parse_basic_header_1byte(std::istream& stream,
                                                          size_t buf_size,
                                                          size_t& readed_size) {
  if (buf_size >= 1) {
    unsigned char header1;

    stream.read(reinterpret_cast<char*>(&header1), sizeof(header1));

    unsigned char format_type = header1 >> 6;
    unsigned short cs_id = header1 & 0x3F;

    parsed_msg_->format_type_ = RtmpHeaderFormat::get_type(format_type);
    if(parsed_msg_->format_type_ == RtmpHeaderFormat::UNKNOWN)
      return false;

    parsed_msg_->chunk_stream_id_ = cs_id;

    if (cs_id == 0) {
      parsing_state_ = BASIC_HEADER_2BYTE;
    } else if (cs_id == 1) {
      parsing_state_ = BASIC_HEADER_3BYTE;
    } else {
      parsing_state_ = get_chunk_header_parsing_state(format_type);
    }

    readed_size = 1;
    buf_size -= readed_size;

    return true;
  } else
    return boost::indeterminate;
}

boost::tribool RtmpHeaderParser::parse_basic_header_2byte(std::istream& stream,
                                                          size_t buf_size,
                                                          size_t& readed_size) {
  if (buf_size >= 1) {
    unsigned char cs_id_minus_64;

    stream.read(reinterpret_cast<char*>(&cs_id_minus_64),
                sizeof(cs_id_minus_64));
    unsigned short cs_id = cs_id_minus_64 + 64;
    parsed_msg_->chunk_stream_id_ = cs_id;

    parsing_state_ = get_chunk_header_parsing_state(parsed_msg_->format_type_);

    readed_size = 1;
    return true;
  } else
    return boost::indeterminate;
}

boost::tribool RtmpHeaderParser::parse_basic_header_3byte(std::istream& stream,
                                                          size_t buf_size,
                                                          size_t& readed_size) {
  if (buf_size >= 2) {
    unsigned short cs_id_field = 0;

    stream.read(reinterpret_cast<char*>(cs_id_field), 2);
    cs_id_field = network_util::ntoh_2(cs_id_field);
    unsigned int cs_id_low = cs_id_field & 0xFF;
    unsigned int cs_id_high = (cs_id_field & 0xFF00) >> 8;

    unsigned int cs_id = cs_id_high * 256 + cs_id_low + 64;
    parsed_msg_->chunk_stream_id_ = cs_id;

    parsing_state_ = get_chunk_header_parsing_state(parsed_msg_->format_type_);

    readed_size = 2;
    return true;
  } else
    return boost::indeterminate;
}

boost::tribool RtmpHeaderParser::parse_chunk_header_type0(std::istream& stream,
                                                          size_t buf_size,
                                                          size_t& readed_size) {
  if (buf_size >= 11) {
    unsigned int timestamp;
    unsigned int msg_length;
    unsigned char msg_type_id;
    unsigned int msg_stream_id;

    stream.read(reinterpret_cast<char*>(&timestamp), 3);
    stream.read(reinterpret_cast<char*>(&msg_length), 3);
    stream.read(reinterpret_cast<char*>(&msg_type_id), 1);
    stream.read(reinterpret_cast<char*>(&msg_stream_id), 4);

    timestamp = network_util::ntoh_3(timestamp);
    msg_length = network_util::ntoh_3(msg_length);
    // msg_stream_id is little_endian
    //msg_stream_id = network_util::ntoh_3(msg_stream_id);

    if (timestamp > 0x00FFFFFF)
      timestamp = 0x00FFFFFF;

    parsed_msg_->timestamp_ = timestamp;
    parsed_msg_->msg_length_ = msg_length;
    parsed_msg_->msg_type_id_ = RtmpHeaderMsgTypeId::get_id(msg_type_id);
    parsed_msg_->msg_stream_id_ = msg_stream_id;

    parsing_state_ =
        (parsed_msg_->timestamp_ == 0x00FFFFFF) ? EXTENDED_TIMESTAMP : COMPLETE;

    if(parsed_msg_->msg_type_id_ == RtmpHeaderMsgTypeId::UNKNOWN)
      return false;

    readed_size = 11;
    return true;
  } else
    return boost::indeterminate;
}

boost::tribool RtmpHeaderParser::parse_chunk_header_type1(std::istream& stream,
                                                          size_t buf_size,
                                                          size_t& readed_size) {
  if (buf_size >= 7) {
    unsigned int timestamp_delta;
    unsigned int msg_length;
    unsigned char msg_type_id;

    stream.read(reinterpret_cast<char*>(&timestamp_delta), 3);
    stream.read(reinterpret_cast<char*>(&msg_length), 3);
    stream.read(reinterpret_cast<char*>(&msg_type_id), 1);

    timestamp_delta = network_util::ntoh_3(timestamp_delta);
    msg_length = network_util::ntoh_3(msg_length);

    parsed_msg_->timestamp_delta_ = timestamp_delta;
    parsed_msg_->msg_length_ = msg_length;
    parsed_msg_->msg_type_id_ =  RtmpHeaderMsgTypeId::get_id(msg_type_id);

    parsing_state_ =
            (parsed_msg_->timestamp_delta_ == 0x00FFFFFF) ? EXTENDED_TIMESTAMP : COMPLETE;

    if(parsed_msg_->msg_type_id_ == RtmpHeaderMsgTypeId::UNKNOWN)
          return false;

    readed_size = 7;
    return true;
  } else
    return boost::indeterminate;
}

boost::tribool RtmpHeaderParser::parse_chunk_header_type2(std::istream& stream,
                                                          size_t buf_size,
                                                          size_t& readed_size) {
  if (buf_size >= 3) {
    unsigned int timestamp_delta;
    stream.read(reinterpret_cast<char*>(&timestamp_delta), 3);

    timestamp_delta = network_util::ntoh_3(timestamp_delta);

    parsed_msg_->timestamp_delta_ = timestamp_delta;
    parsing_state_ = COMPLETE;
    readed_size = 3;
    return true;
  } else
    return boost::indeterminate;
}

boost::tribool RtmpHeaderParser::parse_chunk_header_type3(std::istream& stream,
                                                          size_t buf_size,
                                                          size_t& readed_size) {
  parsing_state_ = COMPLETE;
  return true;
}

boost::tribool RtmpHeaderParser::parse_extended_timestamp(std::istream& stream,
                                                          size_t buf_size,
                                                          size_t& readed_size) {
  if (buf_size >= 4) {
    unsigned int timestamp;
    stream.read(reinterpret_cast<char*>(&timestamp), 4);

    timestamp = network_util::ntoh_4(timestamp);

    parsed_msg_->timestamp_ = timestamp;
    parsing_state_ = COMPLETE;
    readed_size = 4;
    return true;
  } else
    return boost::indeterminate;
}

}  // namespace rtmp_protocol

