#ifndef MESSAGESENDER_HPP_
#define MESSAGESENDER_HPP_
#include "Message.hpp"

namespace rtmp_network {

class MessageSender {
 public:
  MessageSender() {}
  virtual ~MessageSender() {}

  // not networking work, just push message in queue.
  virtual void push_to_send_queue(Message_ptr message) = 0;

  // real network working. all messages in queue will be sended.
  virtual void signal_send_message() = 0;

  virtual void change_continuous_send_state(bool state) = 0;

  virtual unsigned int get_message_sender_id() = 0;
};
// TODO: change to weak_ptr
typedef MessageSender* MessageSender_wptr;
} // namespace rtmp_network

#endif /* MESSAGESENDER_HPP_ */
