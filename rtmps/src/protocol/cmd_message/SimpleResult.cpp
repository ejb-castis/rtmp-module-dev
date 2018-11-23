#include "SimpleResult.hpp"
#include "../../common/StreamUtil.hpp"
#include "../AMF0Serializer.hpp"

namespace rtmp_protocol {

size_t SimpleResult::write_body_payload(std::ostream& stream) {
  size_t writed = 0;
  AMF0Serializer amf0;
  std::string result(CMD_MSG_TYPE_STR_RESULT);
  amf0.write_short_string(stream, result, true, writed);
  amf0.write_number(stream, get_transaction_id(), true, writed);
  amf0.write_null(stream, writed);

  // writed += 1 + 2 + result.length();
  // writed += 1 + 8;
  // writed += 1;

  if(exists_second_object_) {
    amf0.write_number(stream, second_object_, true, writed);
    //writed += 1 + 8;
  } else {
    amf0.write_null(stream, writed);
    //writed += 1;
  }
  return writed;
}

}  //  namespace rtmp_protocol



