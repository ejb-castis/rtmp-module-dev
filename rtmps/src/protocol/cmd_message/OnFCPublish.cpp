#include "OnFCPublish.hpp"
#include "../../common/StreamUtil.hpp"
#include "../AMF0Serializer.hpp"

namespace rtmp_protocol {

size_t OnFCPublish::write_body_payload(std::ostream& stream) {
  size_t writed = 0;
  AMF0Serializer amf0;
  std::string result(CMD_MSG_TYPE_STR_ONFCPUBLISH);
  amf0.write_short_string(stream, result, true, writed);
  amf0.write_number(stream, get_transaction_id(), true, writed);
  amf0.write_null(stream, writed);

  // writed += 1 + 2 + result.length();
  // writed += 1 + 8;
  // writed += 1;

  std::string code(ON_FC_PUBLISH_STR_CODE);
  std::string start(ON_FC_PUBLISH_STR_START);
  std::string description(ON_FC_PUBLISH_STR_DESCRIPTION);

  amf0.write_type(stream, AMF0Type::AMF0_OBJECT, writed);
  amf0.write_object_key(stream, code, writed);
  amf0.write_short_string(stream, start, true, writed);
  amf0.write_object_key(stream, description, writed);
  amf0.write_short_string(stream, stream_name_, true, writed);
  amf0.write_end_of_object(stream, writed);

  // writed += 1;
  // writed += 2 + code.length();
  // writed += 1 + 2 + start.length();
  // writed += 2 + description.length();
  // writed += 1 + 2 + stream_name_.length();
  // writed += 3;

  return writed;
}

}  //  namespace rtmp_protocol

