#include "RtmpSampleAccess.hpp"
#include "../../common/StreamUtil.hpp"
#include "../AMF0Serializer.hpp"

namespace rtmp_protocol {

size_t RtmpSampleAccess::write_body_payload(std::ostream& stream) {
  size_t writed = 0;
  AMF0Serializer amf0;
  std::string result(DATA_MSG_TYPE_STR_RTMPSAMPLEACCESS);
  amf0.write_short_string(stream, result, true, writed);
  amf0.write_boolean(stream, false, true, writed);
  amf0.write_boolean(stream, false, true, writed);

  // writed += 1 + 2 + result.length();
  // writed += 1 + 1;
  // writed += 1 + 1;

  return writed;
}

}  //  namespace rtmp_protocol
