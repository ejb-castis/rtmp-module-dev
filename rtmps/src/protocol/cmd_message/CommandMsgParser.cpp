#include "CommandMsgParser.hpp"
#include "../../common/StreamUtil.hpp"
#include "ConnectMessage.hpp"
#include "ReleaseStream.hpp"
#include "FCPublish.hpp" // and FCUnPublish
#include "CreateStream.hpp" // and DeleteStream
#include "Publish.hpp"
#include "Play.hpp"
#include "../../../src/rtmpmodulelogger.h"

// TODO:
// handing user control message 
// FIXME: 
// handle AMF3 type command 
namespace rtmp_protocol {
bool CommandMsgParser::parse_command_msg(RtmpHeader_ptr header, std::istream& stream,
                             size_t buf_size, size_t& readed_size) {
  size_t available_len_begin = stream_util::get_readable_length(stream);

  
  if(header->msg_type_id_ == RtmpHeaderMsgTypeId::COMMAND_AMF3) {
    if(!stream_util::check_redable_length(stream, 1))
        return false;

    char ignore_char;
    stream_util::read_char(stream, ignore_char);
  }

  std::string command;
  if (!amf0_.read_short_string(stream, command, true))
    return false;

  double transaction_id;
  if (!amf0_.read_number(stream, transaction_id, true))
    return false;

  CommandMsgType::type cmd_type = CommandMsgType::get_type(command);

  bool result = false;
  switch (cmd_type) {
    case CommandMsgType::CONNECT:
      result = parse_connect_msg(stream, transaction_id);
      break;
    case CommandMsgType::RELEASESTREAM:
      result = parse_release_stream(stream, transaction_id);
      break;
    case CommandMsgType::FCPUBLISH:
      result = parse_fc_publish(stream, transaction_id);
      break;
    case CommandMsgType::FCUNPUBLISH:
      result = parse_fc_unpublish(stream, transaction_id);
      break;      
    case CommandMsgType::CREATESTREAM:
      result = parse_create_stream(stream, transaction_id);
      break;
    case CommandMsgType::PUBLISH:
      result = parse_publish(stream, transaction_id);
      break;
    case CommandMsgType::PLAY:
      result = parse_play(stream, transaction_id);
      break;
    case CommandMsgType::DELETESTREAM:
      result = parse_delete_stream(stream, transaction_id);
      break;
    case CommandMsgType::UNKNOWN:
      result = parse_unknown_command(stream, transaction_id, command);
      RTMPLOG(warning) << "parse command: unknown message : " << command;
      break;
  }

  if (!result)
    return false;

  parsed_msg_->set_header(header);

  size_t available_len_end = stream_util::get_readable_length(stream);
  readed_size = available_len_begin - available_len_end;

  return true;
}

// TODO:
// 2018-10-01
// Parse connect command's parameters, if necessary.
bool CommandMsgParser::parse_connect_msg(std::istream& stream,
                                         double transaction_id) {
  AMF0Type::type type;
  if (!amf0_.read_type(stream, type))
    return false;

  if (type != AMF0Type::AMF0_OBJECT)
    return false;

  ConnectMessage_ptr msg(new ConnectMessage);
  msg->set_transaction_id(transaction_id);

  while (true) {
    bool is_not_sufficient_stream_len;
    bool is_end_of_object = amf0_.is_end_of_object(
        stream, is_not_sufficient_stream_len);
    if (is_not_sufficient_stream_len)
      return false;

    if (is_end_of_object)
      break;

    std::string key;
    if (!amf0_.read_short_string(stream, key, false))
      return false;

    ConnectMsgFieldType::type field_type = ConnectMsgFieldType::get_type(key);
    switch (field_type) {
      case ConnectMsgFieldType::APP: {
        if (!amf0_.read_short_string(stream, msg->app_, true))
          return false;
      }
        break;
      case ConnectMsgFieldType::FLASHVER: {
        if (!amf0_.read_short_string(stream, msg->flashver_, true))
          return false;
      }
        break;
      case ConnectMsgFieldType::SWFURL: {
        // read nothing. no need. (usually string type. but sometimes undefined type)
        if (!amf0_.consume_any(stream))
          return false;
      }
        break;
      case ConnectMsgFieldType::TCURL: {
        if (!amf0_.read_short_string(stream, msg->tc_url_, true))
          return false;
      }
        break;
      case ConnectMsgFieldType::FPAD: {
        if (!amf0_.read_boolean(stream, msg->fpad_, true))
          return false;
      }
        break;
      case ConnectMsgFieldType::CAPABILITIES: {
        double double_value;
        if (!amf0_.read_number(stream, double_value, true))
          return false;
        msg->capabilities_ = static_cast<int>(double_value);
      }
        break;
      case ConnectMsgFieldType::AUDIOCODECS: {
        double double_value;
        if (!amf0_.read_number(stream, double_value, true))
          return false;
        msg->audio_codecs_ = static_cast<int>(double_value);
      }
        break;
      case ConnectMsgFieldType::VIDEOCODECS: {
        double double_value;
        if (!amf0_.read_number(stream, double_value, true))
          return false;
        msg->video_codecs_ = static_cast<int>(double_value);
      }
        break;
      case ConnectMsgFieldType::VIDEOFUNCTION: {
        double double_value;
        if (!amf0_.read_number(stream, double_value, true))
          return false;
        msg->video_function_ = static_cast<int>(double_value);
      }
        break;
      case ConnectMsgFieldType::PAGEURL: {
        // read nothing. no need. (usually string type. but sometimes undefined type)
        if (!amf0_.consume_any(stream))
          return false;
      }
        break;
      case ConnectMsgFieldType::OBJECTENCODING: {
        double double_value;
        if (!amf0_.read_number(stream, double_value, true))
          return false;
        msg->object_encoding_ = static_cast<int>(double_value);
      }
        break;
      case ConnectMsgFieldType::UNKNOWN: {
        RTMPLOG(warning) << "connection_id:" << connection_id_ << ",connect_msg:unknown field:" << key;

        if (!amf0_.consume_any(stream))
          return false;
      }
        break;
      default:
        return false;
    }
  }
  set_parsed_msg(msg);

  return true;
}

bool CommandMsgParser::parse_release_stream(std::istream& stream,
                                            double transaction_id) {

  if (!amf0_.read_null(stream))
    return false;

  ReleaseStream_ptr msg(new ReleaseStream);
  msg->set_transaction_id(transaction_id);

  if (!amf0_.read_short_string(stream, msg->stream_name_, true))
    return false;

  set_parsed_msg(msg);

  return true;
}

bool CommandMsgParser::parse_fc_publish(std::istream& stream,
                                        double transaction_id) {
  if (!amf0_.read_null(stream))
    return false;

  FCPublish_ptr msg(new FCPublish);
  msg->set_transaction_id(transaction_id);

  if (!amf0_.read_short_string(stream, msg->stream_name_, true))
    return false;

  set_parsed_msg(msg);

  return true;
}

bool CommandMsgParser::parse_fc_unpublish(std::istream& stream,
                                        double transaction_id) {
  std::string stream_name;
  if (!amf0_.read_null(stream) || !amf0_.read_short_string(stream, stream_name, true)) {
    return false;
  }

  FCUnPublish_ptr msg(new FCUnPublish(transaction_id, stream_name));
  set_parsed_msg(msg);

  return true;
}

bool CommandMsgParser::parse_create_stream(std::istream& stream,
                                           double transaction_id) {
  if (!amf0_.read_null(stream))
    return false;

  CreateStream_ptr msg(new CreateStream);
  msg->set_transaction_id(transaction_id);
  set_parsed_msg(msg);

  return true;
}

bool CommandMsgParser::parse_delete_stream(std::istream& stream, double transaction_id) {
  double stream_id;
  if (!amf0_.read_null(stream) || !amf0_.read_number(stream,stream_id, true)){
    return false;
  }
  DeleteStream_ptr msg(new DeleteStream(transaction_id, stream_id));
  set_parsed_msg(msg);

  return true;
}

bool CommandMsgParser::parse_publish(std::istream& stream,
                                     double transaction_id) {
  if (!amf0_.read_null(stream))
    return false;

  std::string stream_name;
  std::string stream_type;
  if (!amf0_.read_short_string(stream, stream_name, true))
    return false;

  if (!amf0_.read_short_string(stream, stream_type, true))
    return false;

  Publish_ptr msg(new Publish);
  msg->set_transaction_id(transaction_id);
  msg->stream_name_ = stream_name;
  msg->stream_type_ = stream_type;

  set_parsed_msg(msg);

  return true;
}

bool CommandMsgParser::parse_unknown_command(std::istream& stream,
                                             double transaction_id,
                                             std::string& command) {

  if (!amf0_.consume_full(stream))
    return false;

  UnknownCommand_ptr msg(new UnknownCommand);
  msg->set_transaction_id(transaction_id);
  msg->command_name_ = command;

  set_parsed_msg(msg);

  return true;
}

bool CommandMsgParser::parse_play(std::istream& stream,
                                     double transaction_id) {
  if (!amf0_.read_null(stream))
    return false;

  std::string stream_name;
  if (!amf0_.read_short_string(stream, stream_name, true))
    return false;

  Play_ptr msg(new Play);
  msg->set_transaction_id(transaction_id);
  msg->stream_name_ = stream_name;

  set_parsed_msg(msg);

  return true;
}

void CommandMsgParser::set_parsed_msg(RtmpMessage_ptr msg) {
  parsed_msg_.reset();
  parsed_msg_ = msg;
}

}  // namespace rtmp_protocol

