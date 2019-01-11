#ifndef RTMPHANDLER_HPP_
#define RTMPHANDLER_HPP_
#include "../network/RequestHandler.hpp"
#include "../protocol/control_message/ControlMessage.hpp"
#include "../protocol/cmd_message/ConnectMessage.hpp"
#include "../protocol/cmd_message/ReleaseStream.hpp"
#include "../protocol/cmd_message/FCPublish.hpp"
#include "../protocol/cmd_message/CreateStream.hpp"
#include "../protocol/cmd_message/Publish.hpp"
#include "../protocol/cmd_message/Play.hpp"
#include "../protocol/media_message/MediaMessage.hpp"
#include <list>

namespace rtmp_logic {

class RtmpHandler : public rtmp_network::RequestHandler {
 public:
  RtmpHandler(rtmp_network::MessageSender_wptr sender, unsigned int id);
  void handle_request(rtmp_network::Message_ptr request);
  void notify_disconnect();
  void set_context(castis::streamer::media_publish_es_context_ptr);

 private:
  unsigned int get_id();
  void handle_connect_message(rtmp_protocol::ConnectMessage_ptr request);
  void handle_release_stream(rtmp_protocol::ReleaseStream_ptr request);
  void handle_fc_publish(rtmp_protocol::FCPublish_ptr request);
  void handle_fc_unpublish(rtmp_protocol::FCUnPublish_ptr request);
  void handle_create_stream(rtmp_protocol::CreateStream_ptr request);
  void handle_delete_stream(rtmp_protocol::DeleteStream_ptr request);
  void handle_publish(rtmp_protocol::Publish_ptr request);
  void handle_play(rtmp_protocol::Play_ptr request);
  void handle_media_message(rtmp_protocol::MediaMessage_ptr request);
  void handle_set_chunk_size(rtmp_protocol::SetChunkSize_ptr request);
  void handle_on_metadata();

  unsigned int chunk_size_;
  unsigned int id_;
};
typedef boost::shared_ptr<RtmpHandler> RtmpHandler_ptr;

class RtmpHandlerFactory : public rtmp_network::RequestHandlerFactory {
 public:
  RtmpHandlerFactory() {
  }
  virtual ~RtmpHandlerFactory() {
  }
  virtual rtmp_network::RequestHandler_ptr get_request_handler(
      rtmp_network::MessageSender_wptr sender, unsigned int id) {
    return RtmpHandler_ptr(new RtmpHandler(sender, id));
  }
};
typedef boost::shared_ptr<RtmpHandlerFactory> RtmpHandlerFactory_ptr;
}  // namespace rtmp_logic

#endif /* RTMPHANDLER_HPP_ */
