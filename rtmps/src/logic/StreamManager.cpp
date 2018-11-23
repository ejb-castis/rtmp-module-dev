#include "StreamManager.hpp"
#include "../protocol/control_message/ControlMessage.hpp"
#include <algorithm>
#include <boost/bind/bind.hpp>
#include "../../../src/rtmpmodulelogger.h"

namespace rtmp_logic {

StreamManager StreamManager::instance_= StreamManager();

StreamManager* StreamManager::pInstance_ = NULL;

void StreamManager::set_chunk_size(size_t size) {
  write_message(
      rtmp_protocol::SetChunkSize_ptr(new rtmp_protocol::SetChunkSize(size)));
  out_chunk_size_ = size;
}

// TODO: implement StreamManager logic
//void StreamManager::link_stream(InRtmpStream_ptr in, OutRtmpStream_ptr out) {
//  if (in->exists_in_linked_out_streams(out)) {
//    in->register_link(out);
//  }
//}
//RtmpStream_ptr StreamManager::create_stream() {
//  return RtmpStream_ptr(new EmptyRtmpStream());
//}
//
void StreamManager::release_stream(const std::string& stream_name) {

  RTMPLOG(info) << "release stream: " << stream_name ;

  return;

  // std::list<RtmpStream_ptr>::iterator it;
  // it = std::find_if(
  //     stream_list_.begin(), stream_list_.end(),
  //     boost::bind(&RtmpStream::is_same_name, _1, boost::ref(stream_name)));

  // if (it != stream_list_.end()) {
  //   stream_list_.erase(it);
  // }
}


//bool StreamManager::register_in_stream(RtmpStream_ptr stream) {
//  RtmpStream_ptr empty_stream = find_stream(stream);
//  if (!empty_stream)
//    return false;
//
//  if (empty_stream->get_class_name().compare("EmptyStream") != 0) {
//    return false;
//  }
//}
//
//void StreamManager::register_out_stream(RtmpStream_ptr stream) {
//
//}
//void StreamManager::unregister_in_stream(RtmpStream_ptr stream) {
//
//}
//void StreamManager::unregister_out_stream(RtmpStream_ptr stream) {
//
//}
//RtmpStream_ptr StreamManager::find_stream(const std::string& stream_name) {
//  std::list<RtmpStream_ptr>::iterator it;
//  it = std::find_if(
//      stream_list_.begin(), stream_list_.end(),
//      boost::bind(&RtmpStream::is_same_name, _1, boost::ref(stream_name)));
//
//  // not found
//  if (it == stream_list_.end()) {
//    RtmpStream_ptr null_stream;
//    return null_stream;
//  }
//
//  return static_cast<RtmpStream_ptr>(*it);
//}
//
//RtmpStream_ptr StreamManager::find_stream(const RtmpStream_ptr stream) {
//  std::list<RtmpStream_ptr>::iterator it;
//  it = std::find_if(stream_list_.begin(), stream_list_.end(),
//                    boost::bind(&RtmpStream::is_same, _1, boost::ref(stream)));
//
//  // not found
//  if (it == stream_list_.end()) {
//    RtmpStream_ptr null_stream;
//    return null_stream;
//  }
//
//  return static_cast<RtmpStream_ptr>(*it);
//}
//
//void StreamManager::add_media_msg_to_in_stream(
//    rtmp_protocol::MediaMessage_ptr msg) {
//
//}

}  // namespace rtmp_logic
