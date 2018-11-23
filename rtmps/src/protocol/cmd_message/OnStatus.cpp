#include "OnStatus.hpp"
#include "../../common/StreamUtil.hpp"
#include "../AMF0Serializer.hpp"

namespace rtmp_protocol {

size_t OnStatus::write_body_payload(std::ostream& stream) {
  size_t writed = 0;
  AMF0Serializer amf0;
  std::string result(CMD_MSG_TYPE_STR_ONSTATUS);
  amf0.write_short_string(stream, result, true, writed);
  amf0.write_number(stream, get_transaction_id(), true, writed);
  amf0.write_null(stream, writed);

  // writed += 1 + 2 + result.length();
  // writed += 1 + 8;
  // writed += 1;

  std::string level(ON_STATUS_STR_LEVEL);
  std::string status(ON_STATUS_STR_LEVEL_STATUS);
  std::string code(ON_STATUS_STR_CODE);
  std::string description(ON_STATUS_STR_DESCRIPTION);
  std::string details(ON_STATUS_STR_DETAILS);
  std::string clientid(ON_STATUS_STR_CLIENTID);

  std::string code_val = OnStatusCode::get_type_string(code_);
  std::string description_val = OnStatusCode::get_description(code_, stream_name_);

  amf0.write_type(stream, AMF0Type::AMF0_OBJECT, writed);
  amf0.write_object_key(stream, level, writed);
  amf0.write_short_string(stream, status, true, writed);
  amf0.write_object_key(stream, code, writed);
  amf0.write_short_string(stream, code_val, true, writed);
  amf0.write_object_key(stream, description, writed);
  amf0.write_short_string(stream, description_val, true, writed);
  amf0.write_object_key(stream, details, writed);
  amf0.write_short_string(stream, stream_name_, true, writed);
  amf0.write_object_key(stream, clientid, writed);
  amf0.write_short_string(stream, client_id_, true, writed);
  amf0.write_end_of_object(stream, writed);

  // writed += 1;
  // writed += 2 + level.length();
  // writed += 1 + 2 + status.length();
  // writed += 2 + code.length();
  // writed += 1 + 2 + code_val.length();
  // writed += 2 + description.length();
  // writed += 1 + 2 + description_val.length();
  // writed += 2 + details.length();
  // writed += 1 + 2 + stream_name_.length();
  // writed += 2 + clientid.length();
  // writed += 1 + 2 + client_id_.length();
  // writed += 3;

  return writed;
}

}  //  namespace rtmp_protocol



