#include "RtmpStream.hpp"
#include <boost/bind/bind.hpp>
#include <algorithm>

namespace rtmp_logic {
void InRtmpStream::add_data(rtmp_network::Message_ptr msg) {
  std::for_each(
      linked_out_streams_.begin(), linked_out_streams_.end(),
      boost::bind(&OutRtmpStream::write_message, boost::placeholders::_1, boost::ref(msg)));

  // std::for_each(
  //     linked_out_streams_.begin(), linked_out_streams_.end(),
  //     [&msg](OutRtmpStream_ptr& ortmpstream) { ortmpstream->write_message(msg); } );
}

void InRtmpStream::register_link(OutRtmpStream_ptr out) {
  if (!exists_in_linked_out_streams(out)) {
    linked_out_streams_.push_back(out);
  }
}
void InRtmpStream::unregister_link(OutRtmpStream_ptr out) {
  std::list<OutRtmpStream_ptr>::iterator it;
  it = std::find_if(linked_out_streams_.begin(), linked_out_streams_.end(),
                    boost::bind(&OutRtmpStream::is_same, boost::placeholders::_1, boost::ref(out)));
  if (it != linked_out_streams_.end()) {
    linked_out_streams_.erase(it);
  }
}
bool InRtmpStream::exists_in_linked_out_streams(OutRtmpStream_ptr out) {
  std::list<OutRtmpStream_ptr>::iterator it;
  it = std::find_if(linked_out_streams_.begin(), linked_out_streams_.end(),
                    boost::bind(&OutRtmpStream::is_same, boost::placeholders::_1, boost::ref(out)));
  return it != linked_out_streams_.end();
}
}  // namespace rtmp_logic
