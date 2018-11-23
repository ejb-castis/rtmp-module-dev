#include "ControlMsgParser.hpp"
#include "../../common/NetworkUtil.hpp"
#include "../../common/StreamUtil.hpp"
#include "../../../src/rtmpmodulelogger.h"

namespace rtmp_protocol {
bool ControlMsgParser::parse_control_msg(RtmpHeader_ptr header_ptr, std::istream& stream,
                             size_t buf_size, size_t& readed_size) {

  size_t available_len_begin = stream_util::get_readable_length(stream);
  bool result = false;

  switch (header_ptr->msg_type_id_) {
    case RtmpHeaderMsgTypeId::CHUNKSIZE: {
      result = parse_chunk_size(stream);
      break;
    }

    case RtmpHeaderMsgTypeId::ABORTMESSAGE: {
      result = parse_abort_message(stream);
      break;

    }
    case RtmpHeaderMsgTypeId::ACK: {
      result = parse_ack(stream);
      break;
    }

    case RtmpHeaderMsgTypeId::USRCTRL: {
      result = parse_user_ctrl(stream);
      break;
    }
    case RtmpHeaderMsgTypeId::WINACKSIZE: {
      result = parse_win_ack_size(stream);
      break;
    }
    case RtmpHeaderMsgTypeId::PEERBANDWIDTH: {
      result = parse_peer_bandwidth(stream);
      break;
    }
    default: {
      RTMPLOG(error) << "ControlMsgParser[" << connection_id_
                         << "]:unknown control message type:"
                         << header_ptr->msg_type_id_;
      break;
    }
  }
  size_t available_len_end = stream_util::get_readable_length(stream);
  readed_size = available_len_begin - available_len_end;
  return result;
}

bool ControlMsgParser::parse_chunk_size(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 4))
    return false;

  unsigned int field = 0;
  stream_util::read_uint(stream, field, true);

  SetChunkSize_ptr msg(new SetChunkSize(field));
  set_parsed_msg(msg);
  return true;
}

bool ControlMsgParser::parse_abort_message(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 4))
    return false;

  unsigned int field = 0;
  stream_util::read_uint(stream, field, true);

  AbortMessage_ptr msg(new AbortMessage(field));
  set_parsed_msg(msg);
  return true;
}
bool ControlMsgParser::parse_ack(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 4))
    return false;

  unsigned int field = 0;
  stream_util::read_uint(stream, field, true);

  Acknowledgement_ptr msg(new Acknowledgement(field));
  set_parsed_msg(msg);
  return true;

}
bool ControlMsgParser::parse_user_ctrl(std::istream& stream) {
  size_t stream_size = stream_util::get_readable_length(stream);
  if (!stream_util::check_redable_length(stream, 2))
    return false;

  unsigned short field = 0;
  stream_util::read_ushort(stream, field, true);

  UserControlMessage::event_type event_type =
      UserControlMessage::get_event_type(field);

  switch (event_type) {
    case UserControlMessage::EVENT_TYPE_STREAM_BEGIN: {
      return parse_usrctrl_stream_begin(stream);
    }
    case UserControlMessage::EVENT_TYPE_STREAM_EOF: {
      return parse_usrctrl_stream_eof(stream);
    }
    case UserControlMessage::EVENT_TYPE_STREAM_DRY: {
      return parse_usrctrl_stream_dry(stream);
    }
    case UserControlMessage::EVENT_TYPE_SET_BUFFER_LENGTH: {
      return parse_usrctrl_set_buffer_length(stream);
    }
    case UserControlMessage::EVENT_TYPE_STREAM_IS_RECORDED: {
      return parse_usrctrl_stream_is_recorded(stream);
    }
    case UserControlMessage::EVENT_TYPE_PING_REQUEST: {
      return parse_usrctrl_ping_request(stream);
    }
    case UserControlMessage::EVENT_TYPE_PING_RESPONSE: {
      return parse_usrctrl_ping_response(stream);
    }
    case UserControlMessage::EVENT_TYPE_UNKNOWN: {
      RTMPLOG(error) << "ControlMsgParser[" << connection_id_
                         << "]:UserControlMessage:event type is unknown. ("
                         << field << ")";
      return false;
    }
    default:
      return false;
  }

  return true;
}

bool ControlMsgParser::parse_win_ack_size(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 4))
    return false;

  unsigned int field = 0;
  stream_util::read_uint(stream, field, true);

  WindowAcknowledgementSize_ptr msg(new WindowAcknowledgementSize(field));
  set_parsed_msg(msg);
  return true;
}
bool ControlMsgParser::parse_peer_bandwidth(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 5))
    return false;

  unsigned int field = 0;
  unsigned char field2 = 0;
  stream_util::read_uint(stream, field, true);
  stream_util::read_uchar(stream, field2);

  SetPeerBandwidth_ptr msg(
      new SetPeerBandwidth(field, SetPeerBandwidth::get_limit_type(field2)));

  if (msg->limit_type_ == SetPeerBandwidth::LIMIT_TYPE_UNKNOWN) {
    RTMPLOG(error) << "ControlMsgParser[" << connection_id_
                       << "]:SetPeerBandwidth limit_type should be 0 - 2. type("
                       << field2 << ")";
    return false;
  }

  set_parsed_msg(msg);
  return true;
}

bool ControlMsgParser::parse_usrctrl_stream_begin(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 4)) {
    return false;
  }
  unsigned int field;
  stream_util::read_uint(stream, field, true);
  StreamBegin_ptr msg = StreamBegin_ptr(new StreamBegin(field));

  set_parsed_msg(msg);
  return true;
}

bool ControlMsgParser::parse_usrctrl_stream_eof(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 4)) {
    return false;
  }
  unsigned int field;
  stream_util::read_uint(stream, field, true);
  StreamEof_ptr msg = StreamEof_ptr(new StreamEof(field));
  set_parsed_msg(msg);
  return true;
}
bool ControlMsgParser::parse_usrctrl_stream_dry(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 4)) {
    return false;
  }
  unsigned int field;
  stream_util::read_uint(stream, field, true);
  StreamDry_ptr msg = StreamDry_ptr(new StreamDry(field));
  set_parsed_msg(msg);
  return true;
}
bool ControlMsgParser::parse_usrctrl_set_buffer_length(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 8)) {
    return false;
  }
  unsigned int field1, field2;
  stream_util::read_uint(stream, field1, true);
  stream_util::read_uint(stream, field2, true);
  SetBufferLength_ptr msg = SetBufferLength_ptr(
      new SetBufferLength(field1, field2));
  set_parsed_msg(msg);
  return true;
}
bool ControlMsgParser::parse_usrctrl_stream_is_recorded(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 4)) {
    return false;
  }
  unsigned int field;
  stream_util::read_uint(stream, field, true);
  StreamIsRecorded_ptr msg = StreamIsRecorded_ptr(new StreamIsRecorded(field));
  set_parsed_msg(msg);
  return true;
}
bool ControlMsgParser::parse_usrctrl_ping_request(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 4)) {
    return false;
  }
  unsigned int field;
  stream_util::read_uint(stream, field, true);
  PingRequest_ptr msg = PingRequest_ptr(new PingRequest(field));
  set_parsed_msg(msg);
  return true;
}
bool ControlMsgParser::parse_usrctrl_ping_response(std::istream& stream) {
  if (!stream_util::check_redable_length(stream, 4)) {
    return false;
  }
  unsigned int field;
  stream_util::read_uint(stream, field, true);
  PingResponse_ptr msg = PingResponse_ptr(new PingResponse(field));
  set_parsed_msg(msg);
  return true;
}

}  // namespace rtmp_protocol

