#include "DataMsgParser.hpp"
#include "../../common/StreamUtil.hpp"
#include "../../../src/rtmpmodulelogger.h"

namespace rtmp_protocol {
bool DataMsgParser::parse_data_msg(RtmpHeader_ptr header, std::istream& stream,
                             size_t buf_size, size_t& readed_size) {
  size_t available_len_begin = stream_util::get_readable_length(stream);

  std::string command;
  if (!amf0_.read_short_string(stream, command, true)) { 
    return false; 
  }

  // double transaction_id;
  // if (!amf0_.read_number(stream, transaction_id, true))
  //   return false;

  RTMPLOG(debug) << "command string:" << command; 

  if(command.compare("@setDataFrame") == 0) {
    if(!amf0_.consume_full(stream))
    {
      RTMPLOG(debug) << "consume_full fails";

      return false;
    }

    UnknownMessage_ptr msg =  UnknownMessage_ptr(new UnknownMessage);
    set_parsed_msg(msg);

    RTMPLOG(debug) << "unknown msg :" << msg->to_string(); 
  }
  parsed_msg_->set_header(header);

  size_t available_len_end = stream_util::get_readable_length(stream);
  readed_size = available_len_begin - available_len_end;
  return true;
}

void DataMsgParser::set_parsed_msg(RtmpMessage_ptr msg) {
  parsed_msg_.reset();
  parsed_msg_ = msg;
}

}  // namespace rtmp_protocol


