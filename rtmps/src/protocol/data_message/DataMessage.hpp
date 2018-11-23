#ifndef DATAMESSAGE_HPP_
#define DATAMESSAGE_HPP_

#include "../RtmpMessage.hpp"
#include <sstream>

namespace rtmp_protocol {

#define DATA_MSG_TYPE_STR_RTMPSAMPLEACCESS "|RtmpSampleAccess"
#define DATA_MSG_TYPE_STR_ONMETADATA "onMetaData"

class DataMessage : public RtmpMessage {
 public:
  DataMessage() {
    set_msg_type_id(RtmpHeaderMsgTypeId::DATA_AMF0);
  }
  virtual ~DataMessage() {
  }
  virtual std::string get_class_name() {
    return "DataMessage";
  }
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << RtmpMessage::to_string();
    return oss.str();
  }
  virtual size_t write_payload(std::ostream& stream) {
    std::ostringstream body_stream;
    size_t body_len = write_body_payload(body_stream);

    get_header()->msg_length_ = body_len;

    size_t writed_header = get_header()->write_payload(stream);
    size_t writed_body = RtmpMessage::write_payload_with_body(body_stream,
                                                              stream);

    return writed_header + writed_body;
  }

  virtual size_t write_body_payload(std::ostream& stream) = 0;

};

}  // namespace rtmp_protocol

#endif /* DATAMESSAGE_HPP_ */
