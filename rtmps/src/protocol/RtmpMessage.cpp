#include "RtmpMessage.hpp"
#include <sstream>

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

std::string RtmpMessage::to_string() {
    std::ostringstream oss;
    oss << "id[" << id_ <<"],";
    oss << "type[" << header_ptr_->msg_type_id_ <<"],";
    oss << "length[" << header_ptr_->msg_length_ <<"],";
    oss << "timestamp[" << header_ptr_->timestamp_ <<"],";
    oss << "header[" << get_header()->to_string() <<"]";
    return oss.str();
  }
  

}