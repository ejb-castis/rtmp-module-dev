#include "CallError.hpp"
#include "../../common/StreamUtil.hpp"
#include "../AMF0Serializer.hpp"

namespace rtmp_protocol {

size_t CallError::write_body_payload(std::ostream& stream) {
  size_t writed = 0;
  AMF0Serializer amf0;
  std::string result(CMD_MSG_TYPE_STR_ERROR);
  amf0.write_short_string(stream, result, true, writed);
  amf0.write_number(stream, get_transaction_id(), true, writed);
  amf0.write_null(stream, writed);

  // writed += 1 + 2 + result.length();
  // writed += 1 + 8;
  // writed += 1;

  std::string level(CALL_ERROR_STR_LEVEL);
  std::string code(CALL_ERROR_STR_CODE);
  std::string description(CALL_ERROR_STR_DESCRIPTION);
  std::string error(CALL_ERROR_STR_LEVEL_ERROR);
  std::string call_failed(CALL_ERROR_STR_NETCONNECTION_CALL_FAILED);
  std::string stream_not_found(CALL_ERROR_STR_DESCRIPTION_STREAM_NOT_FOUND);

  amf0.write_type(stream, AMF0Type::AMF0_OBJECT, writed);
  amf0.write_object_key(stream, level, writed);
  amf0.write_short_string(stream, error, true, writed);
  amf0.write_object_key(stream, code, writed);
  amf0.write_short_string(stream, call_failed, true, writed);
  amf0.write_object_key(stream, description, writed);
  amf0.write_short_string(stream, stream_not_found, true, writed);
  amf0.write_end_of_object(stream, writed);

  // writed += 1;
  // writed += 2 + level.length();
  // writed += 1 + 2 + error.length();
  // writed += 2 + code.length();
  // writed += 1 + 2 + call_failed.length();
  // writed += 2 + description.length();
  // writed += 1 + 2 + stream_not_found.length();
  // writed += 3;

  return writed;
}

}  //  namespace rtmp_protocol



