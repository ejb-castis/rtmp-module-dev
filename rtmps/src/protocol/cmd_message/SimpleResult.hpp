#ifndef SIMPLERESULT_HPP_
#define SIMPLERESULT_HPP_

#include "CommandMessage.hpp"

#define CALL_ERROR_STR_CODE "code"
#define CALL_ERROR_STR_NETCONNECTION_CALL_FAILED "NetConnection.Call.Failed"
#define CALL_ERROR_STR_DESCRIPTION "description"
#define CALL_ERROR_STR_DESCRIPTION_STREAM_NOT_FOUND "Specified stream not found in call to releaseStream"
#define CALL_ERROR_STR_LEVEL "level"
#define CALL_ERROR_STR_LEVEL_ERROR "error"

namespace rtmp_protocol {

class SimpleResult : public CommandMessage {
 private:
  double second_object_;
  bool exists_second_object_;

 public:
  SimpleResult() {
    set_type(RtmpMessageType::SIMPLE_RESULT);
  }
  SimpleResult(RtmpHeaderFormat::format_type header_type,
               unsigned int chunk_stream_id, unsigned int msg_stream_id,
               double transaction_id, unsigned int write_chunk_size,
               double second_object)
      : second_object_(second_object),
        exists_second_object_(true) {
    set_header_format_type(header_type);
    set_chunk_stream_id(chunk_stream_id);
    set_msg_stream_id(msg_stream_id);
    set_transaction_id(transaction_id);
    set_type(RtmpMessageType::SIMPLE_RESULT);
    set_write_chunk_size(write_chunk_size);
  }
  SimpleResult(RtmpHeaderFormat::format_type header_type,
               unsigned int chunk_stream_id, unsigned int msg_stream_id,
               double transaction_id, unsigned int write_chunk_size)
      : exists_second_object_(false) {
    set_header_format_type(header_type);
    set_chunk_stream_id(chunk_stream_id);
    set_msg_stream_id(msg_stream_id);
    set_transaction_id(transaction_id);
    set_type(RtmpMessageType::SIMPLE_RESULT);
    set_write_chunk_size(write_chunk_size);
  }
  virtual ~SimpleResult() {
  }
  virtual std::string get_class_name() {
    return "SimpleResult";
  }
  virtual size_t write_body_payload(std::ostream& stream);

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "SimpleResult:";
    oss << CommandMessage::to_string();
    if (exists_second_object_)
      oss << ", second_obj:" << second_object_;
    return oss.str();
  }
};
typedef boost::shared_ptr<SimpleResult> SimpleResult_ptr;
}  // namespace rtmp_protocol

#endif /* SIMPLERESULT_HPP_ */
