#ifndef REQUESTHANDLER_HPP_
#define REQUESTHANDLER_HPP_

#include "Message.hpp"
#include "MessageSender.hpp"
#include <boost/shared_ptr.hpp>
#include "flv_message.h"

namespace rtmp_network {
class RequestHandler {
 private:
  MessageSender_wptr sender_;

 public:
  RequestHandler(MessageSender_wptr sender) : sender_(sender){
  }
  virtual ~RequestHandler() {}
  void push_to_send_queue(Message_ptr message) {
    sender_->push_to_send_queue(message);
  }
  void signal_send_message() {
    sender_->signal_send_message();
  }
  void change_continuous_send_state(bool state) {
    sender_->change_continuous_send_state(state);
  }
  MessageSender_wptr get_message_sender() {
    return sender_;
  }
  virtual void handle_request(Message_ptr request) = 0;
  virtual void notify_disconnect() = 0;
 
  castis::streamer::media_publish_es_context_ptr context_;  

};
typedef boost::shared_ptr<RequestHandler> RequestHandler_ptr;

class RequestHandlerFactory {
 public:
  RequestHandlerFactory() {}
  virtual ~RequestHandlerFactory() {}
  virtual RequestHandler_ptr get_request_handler(MessageSender_wptr sender, unsigned int id) = 0;
};
typedef boost::shared_ptr<RequestHandlerFactory> RequestHandlerFactory_ptr;
}  // namespace rtmp_network

#endif /* REQUESTHANDLER_HPP_ */
