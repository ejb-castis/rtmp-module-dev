#include "RtmpHandler.hpp"
#include "../protocol/cmd_message/CallError.hpp"
#include "../protocol/cmd_message/ConnectResult.hpp"
#include "../protocol/cmd_message/OnBWDone.hpp"
#include "../protocol/cmd_message/OnFCPublish.hpp"
#include "../protocol/cmd_message/OnStatus.hpp"
#include "../protocol/cmd_message/SimpleResult.hpp"
#include "../protocol/control_message/ControlMessage.hpp"
#include "../protocol/data_message/OnMetaData.hpp"
#include "../protocol/data_message/RtmpSampleAccess.hpp"
#include "StreamManager.hpp"
#include "rtmpmodulelogger.h"

#include <algorithm>
#include <boost/bind/bind.hpp>
#include <iostream>

#include "publish_to_streamer.h"
#include "rtmpmodulelogger.h"
#include "write_flv.h"

using namespace rtmp_protocol;
namespace rtmp_logic {

const unsigned int NEW_STREAM_ID = 3555;
#define DEFAULT_WIN_ACK_SIZE 5000000

RtmpHandler::RtmpHandler(rtmp_network::MessageSender_wptr sender,
                         unsigned int id)
    : rtmp_network::RequestHandler(sender), chunk_size_(128), id_(id) {}

unsigned int RtmpHandler::get_id() { return id_; }

void RtmpHandler::notify_disconnect() {
  RTMPLOGF(info,
           "connection closed. ready to end stream. "
           "stream_type[%1%],stream_name[%2%],client_id[%3%]",
           context_->stream_type_, context_->stream_name_,
           context_->client_id_);
  context_->ready_to_end_of_stream_ = true;

  // send remains
  publish_to_streamer(context_);
}

void RtmpHandler::handle_request(rtmp_network::Message_ptr request) {
  RtmpMessage_ptr rtmp_msg = boost::dynamic_pointer_cast<RtmpMessage>(request);
  if (rtmp_msg == NULL) {
    RTMPLOG(warning) << "handler_id:" << get_id()
                     << ",this message is not supported"
                     << ",message_info:" << request->to_string();
    return;
  }

  RTMPLOG(info) << "handler_id:" << get_id()
                << ",message_name:" << rtmp_msg->get_class_name()
                << ",message_info:" << rtmp_msg->to_string();

  switch (rtmp_msg->get_type()) {
    case RtmpMessageType::CONNECT: {
      ConnectMessage_ptr msg =
          boost::dynamic_pointer_cast<ConnectMessage>(rtmp_msg);
      handle_connect_message(msg);
      break;
    }
    case RtmpMessageType::RELEASE_STREAM: {
      ReleaseStream_ptr msg =
          boost::dynamic_pointer_cast<ReleaseStream>(rtmp_msg);
      handle_release_stream(msg);
      break;
    }
    case RtmpMessageType::FC_PUBLISH: {
      FCPublish_ptr msg = boost::dynamic_pointer_cast<FCPublish>(rtmp_msg);
      handle_fc_publish(msg);
      break;
    }
    case RtmpMessageType::FC_UNPUBLISH: {
      FCUnPublish_ptr msg = boost::dynamic_pointer_cast<FCUnPublish>(rtmp_msg);
      handle_fc_unpublish(msg);
      break;
    }
    case RtmpMessageType::CREATE_STREAM: {
      CreateStream_ptr msg =
          boost::dynamic_pointer_cast<CreateStream>(rtmp_msg);
      handle_create_stream(msg);
      break;
    }
    case RtmpMessageType::DELETE_STREAM: {
      DeleteStream_ptr msg =
          boost::dynamic_pointer_cast<DeleteStream>(rtmp_msg);
      handle_delete_stream(msg);
      break;
    }
    case RtmpMessageType::PUBLISH: {
      Publish_ptr msg = boost::dynamic_pointer_cast<Publish>(rtmp_msg);
      handle_publish(msg);
      break;
    }
    case RtmpMessageType::PLAY: {
      Play_ptr msg = boost::dynamic_pointer_cast<Play>(rtmp_msg);
      handle_play(msg);
      break;
    }
    case RtmpMessageType::AUDIO:
    case RtmpMessageType::VIDEO: {
      MediaMessage_ptr msg =
          boost::dynamic_pointer_cast<MediaMessage>(rtmp_msg);
      handle_media_message(msg);
      break;
    }
    case RtmpMessageType::SET_CHUNK_SIZE: {
      SetChunkSize_ptr msg =
          boost::dynamic_pointer_cast<SetChunkSize>(rtmp_msg);
      handle_set_chunk_size(msg);
      break;
    }
    case RtmpMessageType::ON_METADATA: {
      handle_on_metadata();
      break;
    }
    default: {
      RTMPLOG(info) << "handler_id:" << get_id()
                    << ",unknown message:" << rtmp_msg->get_class_name();
      break;
    }
  }
  return;
}

void RtmpHandler::handle_connect_message(ConnectMessage_ptr request) {
  WindowAcknowledgementSize_ptr win_ack_size =
      WindowAcknowledgementSize_ptr(new WindowAcknowledgementSize(DEFAULT_WIN_ACK_SIZE));

  SetPeerBandwidth_ptr set_peer_bw = SetPeerBandwidth_ptr(
      new SetPeerBandwidth(DEFAULT_WIN_ACK_SIZE, SetPeerBandwidth::LIMIT_TYPE_DYNAMIC));

  StreamBegin_ptr stream_begin = StreamBegin_ptr(new StreamBegin(0));

  unsigned int chunk_stream_id = request->get_header()->chunk_stream_id_;
  unsigned int msg_stream_id = request->get_header()->msg_stream_id_;
  double transaction_id = request->get_transaction_id();
  double object_encoding = 3;
  unsigned int chunk_size = chunk_size_;

  context_->provider_ = request->app_;

  ConnectResult_ptr connect_result = ConnectResult_ptr(
      new ConnectResult(chunk_stream_id, msg_stream_id, transaction_id,
                        chunk_size, object_encoding));

  // double speed_kbps = 8192;
  // OnBWDone_ptr bw_done = OnBWDone_ptr(
  //     new OnBWDone(RtmpHeaderFormat::SAME_STREAM, chunk_stream_id, 0,
  //                  chunk_size, speed_kbps));

  push_to_send_queue(win_ack_size);
  push_to_send_queue(set_peer_bw);
  push_to_send_queue(stream_begin);
  push_to_send_queue(connect_result);
  // push_to_send_queue(bw_done);
  signal_send_message();

  return;
}

void RtmpHandler::handle_release_stream(ReleaseStream_ptr request) {
  // auto manager = StreamManager::get_instance();
  // manager.release_stream(request->stream_name_);

  RtmpHeaderFormat::format_type header_type = RtmpHeaderFormat::SAME_STREAM;
  unsigned int chunk_stream_id = request->get_header()->chunk_stream_id_;
  unsigned int msg_stream_id = request->get_header()->msg_stream_id_;
  double transaction_id = request->get_transaction_id();
  unsigned int chunk_size = chunk_size_;

  rtmp_network::Message_ptr simple_result = SimpleResult_ptr(new SimpleResult(
      header_type, chunk_stream_id, msg_stream_id, transaction_id, chunk_size));

  push_to_send_queue(simple_result);

  signal_send_message();

  return;
}

void RtmpHandler::handle_fc_publish(FCPublish_ptr request) {
  {  // send response
    RtmpHeaderFormat::format_type header_type = RtmpHeaderFormat::SAME_STREAM;
    unsigned int chunk_stream_id = request->get_header()->chunk_stream_id_;
    unsigned int msg_stream_id = request->get_header()->msg_stream_id_;
    double transaction_id = request->get_transaction_id();
    unsigned int chunk_size = chunk_size_;

    rtmp_network::Message_ptr simple_result = SimpleResult_ptr(
        new SimpleResult(header_type, chunk_stream_id, msg_stream_id,
                         transaction_id, chunk_size));

    push_to_send_queue(simple_result);
  }

  RtmpHeaderFormat::format_type header_type = RtmpHeaderFormat::SAME_STREAM;
  unsigned int chunk_stream_id = request->get_header()->chunk_stream_id_;
  unsigned int msg_stream_id = request->get_header()->msg_stream_id_;
  double transaction_id = 0;
  unsigned int chunk_size = chunk_size_;
  std::string stream_name = request->stream_name_;

  OnFCPublish_ptr on_fc_publish = OnFCPublish_ptr(
      new OnFCPublish(header_type, chunk_stream_id, msg_stream_id,
                      transaction_id, chunk_size, stream_name));

  push_to_send_queue(on_fc_publish);
  signal_send_message();

  return;
}

void RtmpHandler::handle_fc_unpublish(FCUnPublish_ptr request) {
  {  // send response
    RtmpHeaderFormat::format_type header_type = RtmpHeaderFormat::SAME_STREAM;
    unsigned int chunk_stream_id = request->get_header()->chunk_stream_id_;
    unsigned int msg_stream_id = request->get_header()->msg_stream_id_;
    double transaction_id = request->get_transaction_id();
    unsigned int chunk_size = chunk_size_;

    rtmp_network::Message_ptr simple_result = SimpleResult_ptr(
        new SimpleResult(header_type, chunk_stream_id, msg_stream_id,
                         transaction_id, chunk_size));

    push_to_send_queue(simple_result);
  }

  return;
}

void RtmpHandler::handle_create_stream(CreateStream_ptr request) {
  RtmpHeaderFormat::format_type header_type = RtmpHeaderFormat::SAME_STREAM;
  unsigned int chunk_stream_id = request->get_header()->chunk_stream_id_;
  unsigned int msg_stream_id = request->get_header()->msg_stream_id_;
  double transaction_id = request->get_transaction_id();
  unsigned int chunk_size = chunk_size_;

  double new_stream_id = NEW_STREAM_ID;
  context_->stream_id_ = static_cast<unsigned int>(new_stream_id);
  context_->ready_to_end_of_stream_ = false;

  rtmp_network::Message_ptr simple_result = SimpleResult_ptr(
      new SimpleResult(header_type, chunk_stream_id, msg_stream_id,
                       transaction_id, chunk_size, new_stream_id));

  push_to_send_queue(simple_result);
  signal_send_message();

  return;
}

void RtmpHandler::handle_delete_stream(DeleteStream_ptr request) {
  RtmpHeaderFormat::format_type header_type = RtmpHeaderFormat::SAME_STREAM;
  unsigned int chunk_stream_id = request->get_header()->chunk_stream_id_;
  unsigned int msg_stream_id = request->get_header()->msg_stream_id_;
  double transaction_id = request->get_transaction_id();
  unsigned int chunk_size = chunk_size_;
  unsigned int stream_id = request->stream_id();

  context_->ready_to_end_of_stream_ = true;

  rtmp_network::Message_ptr simple_result = SimpleResult_ptr(
      new SimpleResult(header_type, chunk_stream_id, msg_stream_id,
                       transaction_id, chunk_size, stream_id));

  push_to_send_queue(simple_result);
  signal_send_message();

  return;
}

// FIXME: history
// 2018-10-01
// 1. according to spec, send result fro publish message

// 2. remove OnStatus message according to followed guess
// when OnStatus message should be sent ?
// guess when server starts to publish, server should send OnStatus message to
// clients, in this case, client be expected to handle status code in OnStatus
// message with OnStatusCode::ON_STATUS_CODE_PUBLISH_START
//
// TODO:
// when OnStatus message should be sent ?

void RtmpHandler::handle_publish(Publish_ptr request) {
  RtmpHeaderFormat::format_type header_type = RtmpHeaderFormat::FULL;
  
  //FIXME:
  //nginx 처럼 해보기
  unsigned int chunk_stream_id = 5;//request->get_header()->chunk_stream_id_;
  unsigned int msg_stream_id = 1;//request->get_header()->msg_stream_id_;
  
  double transaction_id = request->get_transaction_id();
  unsigned int chunk_size = chunk_size_;

  // rtmp_network::Message_ptr simple_result = SimpleResult_ptr(
  //     new SimpleResult(header_type, chunk_stream_id, msg_stream_id,
  //                      transaction_id, chunk_size, msg_stream_id));
  // push_to_send_queue(simple_result);

  std::string stream_name = request->stream_name_;
  std::string stream_type = request->stream_type_;
  // std::string client_id = stream_type + ":" +  stream_name + ":" +
  // std::to_string(id_);
  std::string client_id = "9_1_38567520";

  context_->stream_name_ = stream_name;
  context_->stream_type_ = stream_type;
  context_->client_id_ = client_id;

  rtmp_network::Message_ptr on_status = OnStatus_ptr(new OnStatus(
      header_type, chunk_stream_id, msg_stream_id, transaction_id, chunk_size,
      OnStatusCode::ON_STATUS_CODE_PUBLISH_START, stream_name, client_id));

  push_to_send_queue(on_status);

  signal_send_message();

  return;
}

void RtmpHandler::handle_play(Play_ptr request) {
  // StreamManager* manager = StreamManager::getInstancePtr();
  // manager->set_out(get_message_sender());
  change_continuous_send_state(true);

  // TODO: process when media data flow begins.
  unsigned int new_stream_id = 1;
  rtmp_network::Message_ptr begin =
      StreamBegin_ptr(new StreamBegin(new_stream_id));

  RtmpHeaderFormat::format_type header_type = RtmpHeaderFormat::FULL;
  unsigned int chunk_stream_id = 20;
  unsigned int msg_stream_id = new_stream_id;
  double transaction_id = 0;
  unsigned int chunk_size = chunk_size_;

  // TODO: get stream name
  // TODO: create client id
  std::string stream_name = "livestream";
  std::string client_id = "9_1_38567520";

  rtmp_network::Message_ptr play_reset = OnStatus_ptr(new OnStatus(
      header_type, chunk_stream_id, msg_stream_id, transaction_id, chunk_size,
      OnStatusCode::ON_STATUS_CODE_PLAY_RESET, stream_name, client_id));

  rtmp_network::Message_ptr play_start = OnStatus_ptr(new OnStatus(
      header_type, chunk_stream_id, msg_stream_id, transaction_id, chunk_size,
      OnStatusCode::ON_STATUS_CODE_PLAY_START, stream_name, client_id));

  rtmp_network::Message_ptr sample_access =
      RtmpSampleAccess_ptr(new RtmpSampleAccess(header_type, chunk_stream_id,
                                                msg_stream_id, chunk_size));

  MediaMetaData meta_data(MediaMetaDataType::DETAIL);
  meta_data.width_ = 320;
  meta_data.height_ = 240;
  meta_data.server_ = "alexrtmp";
  meta_data.bandwidth_ = 98;
  meta_data.description_ = "";
  meta_data.framerate_ = 1;
  meta_data.video_codec_id_ = "avc1";
  meta_data.audio_codec_id_ = ".mp3";
  meta_data.creation_date_ = "Tue Feb 05 17:03:07 2013";
  meta_data.audio_channels_ = 1;
  meta_data.audio_data_rate_ = 48;
  meta_data.audio_device_ = "Realtek High Definition Aud";
  meta_data.audio_input_volume_ = 75;
  meta_data.audio_sample_rate_ = 22050;
  meta_data.author_ = "";
  meta_data.avc_level_ = 31;
  meta_data.avc_profile_ = 66;
  meta_data.copyright_ = "";
  meta_data.keywords_ = "";
  meta_data.preset_name_ = "Custom";
  meta_data.rating_ = "";
  meta_data.title_ = "";
  meta_data.video_data_rate_ = 50;
  meta_data.video_device_ = "Chicony USB 2.0 Camera";
  meta_data.video_key_frame_frequency_ = 5;

  rtmp_network::Message_ptr meta_data_msg = OnMetaData_ptr(new OnMetaData(
      header_type, chunk_stream_id, msg_stream_id, chunk_size, meta_data));

  push_to_send_queue(begin);
  push_to_send_queue(play_reset);
  push_to_send_queue(play_start);
  push_to_send_queue(sample_access);
  push_to_send_queue(meta_data_msg);

  signal_send_message();
  return;
}

// TODO:
// 1. when client publish data, receiving data here.
// publish type : (from spec)
// record : The stream is published and the data is recorded to a new file.
//  The file is stored on the server in a subdirectory within the directory that
//  contains the server application. If thefile already exists, it is
//  overwritten
// append: The stream is published and the data is appended to a file.
//  If no file is found, it is created.
// live: Live data is published without recording it in a file.
//
// Q: who need "live" type publish ? or which case?
//
// TODO:
// when server begin to stream audio, video data, sending data here?
// implement sending data handler somewhere else
// HECK:
// testing

void RtmpHandler::handle_media_message(MediaMessage_ptr request) {
  write_flv(context_, request);
  write_flv_media_message_dump(context_, request);
  publish_to_streamer(context_, request);
}

void RtmpHandler::handle_set_chunk_size(SetChunkSize_ptr request) {
  // StreamManager* manager = StreamManager::getInstancePtr();
  // manager->set_chunk_size(request->chunk_size_);
}

void RtmpHandler::handle_on_metadata() {
  RTMPLOG(debug) << "handle OnMetadata";
}

void RtmpHandler::set_context(
    castis::streamer::media_publish_es_context_ptr ctx) {
  context_ = ctx;
}

}  // namespace rtmp_logic
