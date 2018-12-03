#include <sstream>
#include "RtmpPayloadParser.hpp"
#include "../../../src/rtmpmodulelogger.h"

namespace rtmp_protocol {
RtmpPayloadParser::RtmpPayloadParser()
    : recv_chunk_size_(128) {
}

// TODO:
// code review
// chunk header type 2, 3 need to refer to the msg_stream_Id of the previous chunk header 
// no code for managing and using timestamp ??

RtmpPayloadParseResult::type RtmpPayloadParser::parse_payload(RtmpHeader_ptr header_ptr,
                                                      std::istream& stream,
                                                      size_t buf_size,
                                                      size_t& readed_size) {
  const unsigned int cs_id = header_ptr->chunk_stream_id_;

  // TODEBUG:
  unsigned int current_msg_stream_Id = header_ptr->msg_stream_id_;

  received_payload_info_ptr recv_info;
  parsing_info_map::const_iterator recv_info_iter = incomplete_infos_.find(cs_id);

  if (recv_info_iter == incomplete_infos_.end()) {
    
    recv_info = received_payload_info_ptr(new received_payload_info);
    recv_info->first_chunk_header = RtmpHeader_ptr(new RtmpHeader(header_ptr.get()));

    // TODO:
    // 같은 chunk 에 대해서 message stream id 가 다른 0 type 이 하나 더 왔을 때, 이전 정보를 덮어쓰게 될 텐데... 괜찮을까?
    // 즉, message stream id 에 대한 정보가 없어도 될까? 
    // message header type 0 과 message header type `의 경우, 
    if (recv_info->first_chunk_header->has_msg_len_and_type()) {
      channels_[cs_id].last_msg_len = recv_info->first_chunk_header->msg_length_;
      channels_[cs_id].last_msg_type_id = recv_info->first_chunk_header->msg_type_id_;
    } else {
      recv_info->first_chunk_header->msg_length_ = channels_[cs_id].last_msg_len;
      recv_info->first_chunk_header->msg_type_id_ = channels_[cs_id].last_msg_type_id;
    }

    recv_info->recv_buf = new unsigned char[recv_info->first_chunk_header->msg_length_];
    recv_info->recv_length = 0;

    // timestamp 계산
    if (recv_info->first_chunk_header->format_type_ == RtmpHeaderFormat::FULL) {
      if (recv_info->first_chunk_header->timestamp_delta_ != 0 ) { 
        RTMPLOGF(warning, "timestamp delta of message header(type0) is not zero. header[%1%]", recv_info->first_chunk_header->to_string()); }

      channels_[cs_id].last_msg_timestamp_delta = recv_info->first_chunk_header->timestamp_delta_;
      channels_[cs_id].last_msg_timestamp = recv_info->first_chunk_header->timestamp_;


    }
    else if (recv_info->first_chunk_header->format_type_ == RtmpHeaderFormat::SAME_STREAM) {
      channels_[cs_id].last_msg_timestamp_delta = recv_info->first_chunk_header->timestamp_delta_;
      channels_[cs_id].last_msg_timestamp += channels_[cs_id].last_msg_timestamp_delta;

      recv_info->first_chunk_header->timestamp_ = channels_[cs_id].last_msg_timestamp;
    }
    else if (recv_info->first_chunk_header->format_type_ == RtmpHeaderFormat::SAME_LENGTH_AND_STREAM) {
      channels_[cs_id].last_msg_timestamp_delta = recv_info->first_chunk_header->timestamp_delta_;
      channels_[cs_id].last_msg_timestamp += channels_[cs_id].last_msg_timestamp_delta;

      recv_info->first_chunk_header->timestamp_ = channels_[cs_id].last_msg_timestamp;
    }
    else if (recv_info->first_chunk_header->format_type_ == RtmpHeaderFormat::CONTINUATION) {
      if (channels_[cs_id].last_msg_timestamp_delta == 0 ) { 
        RTMPLOGF(warning, "preceding timestamp delta is zero for new message. header[%1%]", recv_info->first_chunk_header->to_string()); }

      recv_info->first_chunk_header->timestamp_delta_ = channels_[cs_id].last_msg_timestamp_delta;
      channels_[cs_id].last_msg_timestamp_delta = recv_info->first_chunk_header->timestamp_delta_;
      channels_[cs_id].last_msg_timestamp += channels_[cs_id].last_msg_timestamp_delta;
     
      recv_info->first_chunk_header->timestamp_ = channels_[cs_id].last_msg_timestamp;
    }

  } else {
    recv_info = recv_info_iter->second;

    // TODEBUG: message id 가 다른 경우가 있을까?
    if (current_msg_stream_Id != recv_info->first_chunk_header->msg_stream_id_) {
      RTMPLOG(debug) << "current message stream id:" << current_msg_stream_Id << "current header info:" << header_ptr->to_string() << ",previous header info:" << recv_info->first_chunk_header->to_string();
    }
  }

  const RtmpHeader_ptr first_header = recv_info->first_chunk_header;
  const unsigned int msg_length = first_header->msg_length_;
  const unsigned int need_size = msg_length - recv_info->recv_length;
  const unsigned int this_msg_size =
      (need_size > recv_chunk_size_) ? recv_chunk_size_ : need_size;

  //RTMPLOG(debug) << "message length: " << msg_length << ", current msg_length - previously received_length: " << need_size << ", this msg size:" << this_msg_size << ", buf size: "<< buf_size;

  if (buf_size >= this_msg_size) {
    //RTMPLOG(debug) << "read payload from buf(size: " << buf_size << ") READ stream: size:" <<this_msg_size ;

    stream.read(
        reinterpret_cast<char*>(recv_info->recv_buf + recv_info->recv_length),
        this_msg_size);

#ifdef PAYLOAD_RAW_DATA_PRINT
    {
      std::ostringstream oss;
      oss << "receieved payload data:";

      size_t len = this_msg_size;
      oss << std::hex << std::setfill('0');
      for(size_t i = 0; i < len; i++) {
        oss << std::setw(2) << static_cast<unsigned int>(recv_info->recv_buf[recv_info->recv_length + i]) << " ";
      }
      std::cout << oss.str() << std::endl;
    }
#endif

    recv_info->recv_length += this_msg_size;
    readed_size = this_msg_size;

  } else {
    //RTMPLOG(debug) << "WAITING_MORE_PAYLOAD_DATA : " << "current buf size: " << buf_size << ", until buf size is greater than message length: " << this_msg_size ;
    return RtmpPayloadParseResult::WAITING_MORE_PAYLOAD_DATA;
  }

// msg complete
  if (this_msg_size == need_size) {

    // detail parsing
    std::string str_buf(reinterpret_cast<const char*>(recv_info->recv_buf),
                        msg_length);
    std::istringstream payload_stream(str_buf);

    if (first_header->is_control_msg()) {
      size_t readed_in_payload_parse;
      control_msg_parser_.parse_control_msg(first_header, payload_stream, msg_length,
                                readed_in_payload_parse);
      set_parsed_msg(control_msg_parser_.get_parsed_msg());
    } else if (first_header->is_command_msg()) {
      size_t readed_in_payload_parse;
      if (!command_msg_parser_.parse_command_msg(first_header, payload_stream, msg_length,
                                     readed_in_payload_parse))
        return RtmpPayloadParseResult::FAIL;
      set_parsed_msg(command_msg_parser_.get_parsed_msg());
    } else if (first_header->is_data_msg()) {
      size_t readed_in_payload_parse;
      if (!data_msg_parser_.parse_data_msg(first_header, payload_stream, msg_length,
                                  readed_in_payload_parse))
        return RtmpPayloadParseResult::FAIL;
      set_parsed_msg(data_msg_parser_.get_parsed_msg());
    } else if (first_header->is_video_msg() || first_header->is_audio_msg()) {
      size_t readed_in_payload_parse;
      if (!media_msg_parser_.parse_media_msg(first_header, payload_stream, msg_length,
                                   readed_in_payload_parse))
        return RtmpPayloadParseResult::FAIL;
      set_parsed_msg(media_msg_parser_.get_parsed_msg());

    } else {
      parse_unknown_message(first_header, payload_stream, msg_length);
    }

    RTMPLOGF(report, "message[%1%]", parsed_msg_->to_string());

    delete[] recv_info->recv_buf;
    incomplete_infos_.erase(cs_id);
    return RtmpPayloadParseResult::COMPLETE;
  }
// msg incomplete : // message length 가  chunk size 보다 커서, 여러 chunk 를 읽어야 하는 경우
  else {
    incomplete_infos_.insert(parsing_info_pair(cs_id, recv_info));
    return RtmpPayloadParseResult::WAITING_NEXT_CHUNK;
  }
}

bool RtmpPayloadParser::parse_unknown_message(RtmpHeader_ptr header_ptr,
                                              std::istream& stream,
                                              size_t buf_size) {
  unsigned char data[buf_size];
  stream.read(reinterpret_cast<char*>(data), sizeof(data));

  std::string str(reinterpret_cast<const char*>(data), sizeof(data));
  UnknownMessage_ptr msg(new UnknownMessage);
  msg->buf_ = str;
  set_parsed_msg(msg);
  return true;
}

void RtmpPayloadParser::set_parsed_msg(RtmpMessage_ptr msg) {
  parsed_msg_.reset();
  parsed_msg_ = msg;
}
}  // namespace rtmp_protocol
