#include "OnBWDone.hpp"
#include "../../common/StreamUtil.hpp"
#include "../AMF0Serializer.hpp"

namespace rtmp_protocol {

size_t OnBWDone::write_body_payload(std::ostream& stream) {
  size_t writed = 0;
  AMF0Serializer amf0;
  std::string result(CMD_MSG_TYPE_STR_ONBWDONE);
  amf0.write_short_string(stream, result, true, writed);
  amf0.write_number(stream, get_transaction_id(), true, writed);
  amf0.write_null(stream, writed);
  amf0.write_number(stream, speed_kbps_, true, writed);

  // writed += 1 + 2 + result.length();
  // writed += 1 + 8;
  // writed += 1;
  // writed += 1 + 8;

  return writed;
}

}  //  namespace rtmp_protocol
