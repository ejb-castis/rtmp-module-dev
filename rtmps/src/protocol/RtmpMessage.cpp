#include "RtmpMessage.hpp"

namespace rtmp_protocol {

static std::size_t uid{0};

RtmpMessage::RtmpMessage() {
    id_ = ++uid;
    header_ptr_ = RtmpHeader_ptr(new RtmpHeader());
    header_ptr_->message_id_ = id_;     
  }
RtmpMessage::RtmpMessage(RtmpHeader_ptr header)
      : header_ptr_(header) {
    id_ = ++uid;   
    header_ptr_->message_id_ = id_;     
  }
  
}