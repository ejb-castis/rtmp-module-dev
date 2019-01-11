#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <boost/logic/tribool.hpp>
#include <iostream>
#include "Message.hpp"
#include "MessageSender.hpp"
#include <boost/shared_ptr.hpp>
#include "flv_message.h"

namespace rtmp_network {
class RequestParser {
 private:
  MessageSender_wptr sender_;

 public:
  RequestParser() {
  }
  RequestParser(MessageSender_wptr sender)
      : sender_(sender) {
  }
  virtual ~RequestParser() {
  }

  void push_to_send_queue(Message_ptr message) {
    sender_->push_to_send_queue(message);
  }
  void signal_send_message() {
    sender_->signal_send_message();
  }
  void change_continuous_send_state(bool state) {
    sender_->change_continuous_send_state(state);
  }
  virtual void set_context(castis::streamer::media_publish_es_context_ptr) = 0;
  castis::streamer::media_publish_es_context_ptr context_;  

  // return value :
  // 1. true : message parsing complete
  // 2. false : parsing error
  // 3. boost::indeterminate : need more read operation
  virtual boost::tribool parse(std::istream& request_stream, size_t buf_size,
                               size_t& readed_size) = 0;
  virtual Message_ptr get_parsed_message() = 0;
};
typedef boost::shared_ptr<RequestParser> RequestParser_ptr;

class RequestParserFactory {
 public:
  RequestParserFactory() {}
  virtual ~RequestParserFactory() {}
  virtual RequestParser_ptr get_request_parser(MessageSender_wptr sender, unsigned int id) = 0;
};
typedef boost::shared_ptr<RequestParserFactory> RequestParserFactory_ptr;
}
#endif /* PARSER_HPP_ */
