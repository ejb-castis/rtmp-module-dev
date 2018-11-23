#ifndef DATAMSGPARSER_HPP_
#define DATAMSGPARSER_HPP_

#include <iostream>
#include "DataMessage.hpp"
#include "../RtmpHeader.hpp"
#include "../AMF0Serializer.hpp"

namespace rtmp_protocol {

class DataMsgParser {
 public:
  DataMsgParser() {
  }
  virtual ~DataMsgParser() {
  }
  bool parse_data_msg(RtmpHeader_ptr header, std::istream& stream, size_t buf_size,
             size_t& readed_size);
  RtmpMessage_ptr get_parsed_msg() {
    return parsed_msg_;
  }
  void set_connection_id(unsigned int id) {
    connection_id_ = id;
  }
 private:
  bool parse_unknown_data(std::istream& stream, double transaction_id,
                          std::string& command);
  void set_parsed_msg(RtmpMessage_ptr msg);
  RtmpMessage_ptr parsed_msg_;
  AMF0Serializer amf0_;
  unsigned int connection_id_;
};

}  // namespace rtmp_protocol

#endif /* DATAMSGPARSER_HPP_ */
