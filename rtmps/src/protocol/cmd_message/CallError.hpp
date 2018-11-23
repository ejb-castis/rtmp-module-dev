#ifndef CALLERROR_HPP_
#define CALLERROR_HPP_

#include "CommandMessage.hpp"

#define CALL_ERROR_STR_CODE "code"
#define CALL_ERROR_STR_NETCONNECTION_CALL_FAILED "NetConnection.Call.Failed"
#define CALL_ERROR_STR_DESCRIPTION "description"
#define CALL_ERROR_STR_DESCRIPTION_STREAM_NOT_FOUND "Specified stream not found in call to releaseStream"
#define CALL_ERROR_STR_LEVEL "level"
#define CALL_ERROR_STR_LEVEL_ERROR "error"

namespace rtmp_protocol {

class CallError : public CommandMessage {
 public:
  CallError() {
    set_type(RtmpMessageType::CALL_ERROR);
  }
  CallError(RtmpHeaderFormat::format_type header_type,
            unsigned int chunk_stream_id, unsigned int msg_stream_id,
            double transaction_id, unsigned int write_chunk_size) {
    set_header_format_type(header_type);
    set_chunk_stream_id(chunk_stream_id);
    set_msg_stream_id(msg_stream_id);
    set_transaction_id(transaction_id);
    set_type(RtmpMessageType::CALL_ERROR);
    set_write_chunk_size(write_chunk_size);
  }
  virtual ~CallError() {
  }
  virtual std::string get_class_name() {
    return "CallError";
  }
  virtual size_t write_body_payload(std::ostream& stream);

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "CallError:";
    oss << CommandMessage::to_string();
    return oss.str();
  }
};
typedef boost::shared_ptr<CallError> CallError_ptr;
}  // namespace rtmp_protocol

#endif /* CALLERROR_HPP_ */
