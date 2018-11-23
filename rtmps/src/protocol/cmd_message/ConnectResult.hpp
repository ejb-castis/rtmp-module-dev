#ifndef CONNECTRESULT_HPP_
#define CONNECTRESULT_HPP_

#include "CommandMessage.hpp"

#define CONNECT_RESULT_STR_CODE "code"
#define CONNECT_RESULT_STR_NETCONNECTION_REJECTED "NetConnection.Connect.Rejected"
#define CONNECT_RESULT_STR_NETCONNECTION_SUCCESS "NetConnection.Connect.Success"
#define CONNECT_RESULT_STR_DESCRIPTION "description"
#define CONNECT_RESULT_STR_DESCRIPTION_CONNECTION_SUCCEEDED "Connection succeeded"
#define CONNECT_RESULT_STR_LEVEL "level"
#define CONNECT_RESULT_STR_LEVEL_ERROR "error"
#define CONNECT_RESULT_STR_LEVEL_STATUS "status"
#define CONNECT_RESULT_STR_OBJECT_ENCODING "objectEncoding"
#define CONNECT_RESULT_STR_FMS_VER "fmsVer"
#define CONNECT_RESULT_STR_FMS_301123 "FMS/3,0,1,123"
#define CONNECT_RESULT_STR_CAPABILITIES "capabilities"

namespace rtmp_protocol {

class ConnectResult : public CommandMessage {
 private:
  double object_encoding_;

 public:
  ConnectResult() {
    set_type(RtmpMessageType::CONNECT_RESULT);
  }
  ConnectResult(unsigned int chunk_stream_id, unsigned int msg_stream_id,
                double transaction_id, unsigned int write_chunk_size,
                double object_encoding)
      : object_encoding_(object_encoding) {
    set_chunk_stream_id(chunk_stream_id);
    set_msg_stream_id(msg_stream_id);
    set_transaction_id(transaction_id);
    set_type(RtmpMessageType::CONNECT_RESULT);
    set_write_chunk_size(write_chunk_size);
  }
  virtual ~ConnectResult() {
  }
  virtual std::string get_class_name() {
      return "ConnectResult";
  }
  virtual size_t write_body_payload(std::ostream& stream);

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "ConnectResult:";
    oss << CommandMessage::to_string();
    oss << ", object_encoding:" << object_encoding_;
    return oss.str();
  }
};
typedef boost::shared_ptr<ConnectResult> ConnectResult_ptr;
}  // namespace rtmp_protocol
#endif /* CONNECTRESULT_HPP_ */
