#include "ConnectResult.hpp"
#include "../../common/StreamUtil.hpp"
#include "../AMF0Serializer.hpp"

namespace rtmp_protocol {

size_t ConnectResult::write_body_payload(std::ostream& stream) {
  size_t writed = 0;
  AMF0Serializer amf0;
  std::string result(CMD_MSG_TYPE_STR_RESULT);
  amf0.write_short_string(stream, result, true, writed);
  amf0.write_number(stream, get_transaction_id(), true, writed);

  // writed += 1 + 2 + result.length();
  // writed += 1 + 8;

  std::string fms_ver(CONNECT_RESULT_STR_FMS_VER);
  std::string fms_value(CONNECT_RESULT_STR_FMS_301123);
  std::string capabilities(CONNECT_RESULT_STR_CAPABILITIES);

  amf0.write_type(stream, AMF0Type::AMF0_OBJECT, writed);

  amf0.write_object_key(stream, fms_ver, writed);
  amf0.write_short_string(stream, fms_value, true, writed);
  amf0.write_object_key(stream, capabilities, writed);
  amf0.write_number(stream, 31, true, writed);

  amf0.write_end_of_object(stream, writed);

  // writed += 1;
  // writed += 2 + fms_ver.length();
  // writed += 1 + 2 + fms_value.length();
  // writed += 2 + capabilities.length();
  // writed += 1 + 8;
  // writed += 3;

  std::string level(CONNECT_RESULT_STR_LEVEL);
  std::string status(CONNECT_RESULT_STR_LEVEL_STATUS);
  std::string code(CONNECT_RESULT_STR_CODE);
  std::string success(CONNECT_RESULT_STR_NETCONNECTION_SUCCESS);
  std::string description(CONNECT_RESULT_STR_DESCRIPTION);
  std::string succeed(CONNECT_RESULT_STR_DESCRIPTION_CONNECTION_SUCCEEDED);
  std::string encoding(CONNECT_RESULT_STR_OBJECT_ENCODING);

  amf0.write_type(stream, AMF0Type::AMF0_OBJECT, writed);

  amf0.write_object_key(stream, level, writed);
  amf0.write_short_string(stream, status, true, writed);
  amf0.write_object_key(stream, code, writed);
  amf0.write_short_string(stream, success, true, writed);
  amf0.write_object_key(stream, description, writed);
  amf0.write_short_string(stream, succeed, true, writed);
  amf0.write_object_key(stream, encoding, writed);
  amf0.write_number(stream, object_encoding_, true, writed);

  amf0.write_end_of_object(stream, writed);

  // writed += 1;
  // writed += 2 + level.length();
  // writed += 1 + 2 + status.length();
  // writed += 2 + code.length();
  // writed += 1 + 2 + success.length();
  // writed += 2 + description.length();
  // writed += 1 + 2 + succeed.length();
  // writed += 2 + encoding.length();
  // writed += 1 + 8;
  // writed += 3;

  return writed;
}

}  //  namespace rtmp_protocol
