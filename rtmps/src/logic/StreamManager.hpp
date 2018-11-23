#ifndef STREAMMANAGER_HPP_
#define STREAMMANAGER_HPP_

#include "../protocol/RtmpMessage.hpp"
#include "../protocol/media_message/MediaMessage.hpp"
#include "../network/MessageSender.hpp"
#include "RtmpStream.hpp"
#include <list>

namespace rtmp_logic {

// singleton
class StreamManager {
 private:
  static StreamManager *pInstance_;
  rtmp_network::MessageSender_wptr out_;
  unsigned int out_chunk_size_;


  static StreamManager instance_;
  

// TODO: implement StreamManager logic
//  std::list<RtmpStream_ptr> stream_list_;

 public:
  StreamManager()
      : out_(NULL),
        out_chunk_size_(128) {
  }

  static StreamManager& get_instance() {
    return instance_;
  }

  static StreamManager* getInstancePtr() {
    if (pInstance_ == NULL)
      pInstance_ = new StreamManager;
    return pInstance_;
  }
  static void releaseInstance() {
    if (pInstance_ != NULL)
      delete pInstance_;
  }

  void set_out(rtmp_network::MessageSender_wptr out) {
    out_ = out;
  }
  rtmp_network::MessageSender_wptr get_out() {
    return out_;
  }
  void write_message(rtmp_protocol::RtmpMessage_ptr msg) {
    if (out_ != NULL) {
      msg->set_write_chunk_size(out_chunk_size_);
      out_->push_to_send_queue(msg);
    } else {
    }
  }
  void set_chunk_size(size_t size);

  // TODO: implement StreamManager logic
//  void link_stream(InRtmpStream_ptr in, OutRtmpStream_ptr out);
//  RtmpStream_ptr create_stream();
void release_stream(const std::string& stream_name);
//  bool register_in_stream(RtmpStream_ptr stream);
//  void register_out_stream(RtmpStream_ptr stream);
//  void unregister_in_stream(RtmpStream_ptr stream);
//  void unregister_out_stream(RtmpStream_ptr stream);
//  RtmpStream_ptr find_stream(const std::string& stream_name);
//  RtmpStream_ptr find_stream(const RtmpStream_ptr stream);
//  void add_media_msg_to_in_stream(rtmp_protocol::MediaMessage_ptr msg);
};
}  // namespace rtmp_logic

#endif /* STREAMMANAGER_HPP_ */
