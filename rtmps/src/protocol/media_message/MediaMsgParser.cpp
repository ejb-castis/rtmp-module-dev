#include "MediaMsgParser.hpp"
#include "MediaMessage.hpp"
#include "../../../src/rtmpmodulelogger.h"

namespace rtmp_protocol {
bool MediaMsgParser::parse_media_msg(RtmpHeader_ptr header, std::istream& stream,
                           size_t buf_size, size_t& readed_size) {
  size_t available_len_begin = stream_util::get_readable_length(stream);

  uchar_array buf = uchar_array(new unsigned char[buf_size]);
  stream.read(reinterpret_cast<char*>(buf.get()), buf_size);

  if (header->msg_type_id_ == RtmpHeaderMsgTypeId::VIDEODATA) {
    VideoMessage_ptr msg = VideoMessage_ptr(new VideoMessage);
    msg->set_data(buf, buf_size);
    msg->set_header(header);
    set_parsed_msg(msg);

  } else {
    AudioMessage_ptr msg = AudioMessage_ptr(new AudioMessage);
    msg->set_data(buf, buf_size);
    msg->set_header(header);
    set_parsed_msg(msg);

  }

  size_t available_len_end = stream_util::get_readable_length(stream);
  readed_size = available_len_begin - available_len_end;
  

  return true;
}

void MediaMsgParser::set_parsed_msg(RtmpMessage_ptr msg) {
  parsed_msg_.reset();
  parsed_msg_ = msg;
}

}  // namespace rtmp_protocol

