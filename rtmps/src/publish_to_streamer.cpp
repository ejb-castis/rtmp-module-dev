#include <chrono>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <string>

#include "rtmpmodulelogger.h"
#include "flv_message.h"

#include "HTTPAsyncClient.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "streamer/base64.h"

#include "publish_to_streamer.h"

namespace rtmp_logic {

void begin_publish(std::string const& url, castis::streamer::media_publish_es_context_ptr& context, std::string& publish_addr, std::string& upload_id, int& ec)
{
  ec = 0;
  castis::http::Request req(url);
	req.SetMethod("POST");
  req.header()["HOST"] = "flv-es-uploader";
	req.header()["User-Agent"] = "flv-es-uploader";
  req.header()["content-type"] = "application/json";

  std::string f = R"_({
    "filename" : "%s", "assetId" : "%s", "title" : "%s", 
    "provider": "%s", "type" : "%s", "recording" : "%s"
  })_";

  std::string recording = (context->recording_ == 1) ? "Y":"N";

  std::string json = (boost::format(f) 
    % context->stream_name_ % context->stream_name_ % context->stream_name_ 
    % context->provider_ % std::string("Video") % recording).str();

  req.addBody(json);
  std::cout << "start publish>" << std::endl;
  std::cout << "request string>" << std::endl;
  std::cout << req.string() << std::endl;

  castis::http::AsyncClient client;
	castis::http::Response res;
	if ( client.SendAndReceive(req, &res) == false ) {
    std::cout << "send and receive error" << std::endl;
    ec= -1;
    return;
  }
  if (res.status_code() < 200 || res.status_code() >= 300) {
    std::cout << "response error[" << res.status_code() <<"]"<< std::endl;
    ec= res.status_code();
    return;
  }

	std::size_t nread = res.content_length();
  char* pBuffer = new char[nread];
	memcpy(pBuffer, &res.body()[0], nread);
  std::string response_body(pBuffer, nread);

  std::cout << "content >" << std::endl;
  std::cout << response_body << std::endl;

  rapidjson::Document doc;
  doc.Parse(response_body.c_str());
  if (doc.HasParseError()) {
    std::cout << "failed to parse upload start response, "
              << rapidjson::GetParseError_En(doc.GetParseError()) << std::endl;
    ec = 1;              
    return;
  }
  auto pos = doc.FindMember("id");
  if (doc.MemberEnd() == pos || false == pos->value.IsString()) {
    std::cout << "failed to parse upload start response, not exists id"
              << std::endl;
    ec = 2;              
    return;
  }
  auto pos_addr = doc.FindMember("publishAddr");
  if (doc.MemberEnd() == pos_addr || false == pos_addr->value.IsString()) {
    std::cout << "failed to parse upload start response, not exists publishAddr"
              << std::endl;
    ec = 3;              
    return;              
  }

  upload_id  = pos->value.GetString();
  std::cout << "upload_id[" << upload_id << "]" << std::endl;

  publish_addr  = pos_addr->value.GetString();
  std::cout << "publish_addr[" << publish_addr << "]" << std::endl;
}

void init_publish(castis::http::AsyncClient* client, castis::streamer::media_publish_es_context_ptr& context, int& ec)
{
  ec=0;
  std::string publish_url = "http://" + context->publish_es_addr_port_ + 
    "/publish/es/" + context->publish_id_ + "/init";
  
  castis::http::Request req(publish_url);
  req.SetMethod("POST");
  req.header()["HOST"] = "flv-es-uploader";
  req.header()["User-Agent"] = "flv-es-uploader";
  req.header()["content-type"] = "application/json";

  std::string avc, aac;
  {
    auto buf = reinterpret_cast<const char*>(context->video_init_es_.codecInfo.data());
    std::string avc_data(buf, context->video_init_es_.codecInfo.size());
    if (Base64::Encode(avc_data, &avc) == false) {
      std::cout << "failed to encode base64 with avc codec info" << std::endl;
      ec = 3;
      return;
    }
  }

  {
    auto buf = reinterpret_cast<const char*>(context->audio_init_es_.codecInfo.data());
    std::string aac_data(buf, context->audio_init_es_.codecInfo.size());
    if (Base64::Encode(aac_data, &aac) == false) {
      std::cout << "failed to encode base64 with avc codec info" << std::endl;
      ec = 3;
      return;
    }
  }

  std::string f = R"_({
      "tracks" : [
        {"trackId" : 1, "trackType" : "video", "timescale" : 90000, "codec": "h264", "codecInfo" : "%s"},
        {"trackId" : 2, "trackType" : "audio", "timescale" : 90000, "codec": "aac", "codecInfo" : "%s"}
      ]
  })_";

  std::string json = (boost::format(f) % avc.c_str() % aac.c_str()).str();
  req.addBody(json);

  std::cout << "init publish>" << publish_url << std::endl;
  std::cout << "request string>" << std::endl;
  std::cout << req.string() << std::endl;

	castis::http::Response res;

  if ( client->SendAndReceive(req, &res) == false ) {
    std::cout << "send and receive error" << std::endl;
    ec=1;
    return ;
  }
  if (res.status_code() < 200 || res.status_code() >= 300) {
    std::cout << "response error[" << res.status_code() <<"]"<< std::endl;
    ec=res.status_code();
    return ;
  }
 
}

void publish_es(castis::http::AsyncClient* client, castis::streamer::media_publish_es_context_ptr& context, int& ec)
{
  ec = 0;
  if (context->media_es_.empty()) return;

  auto es = context->media_es_.front().get();

  std::vector<unsigned char> track_id = flv_util::to_vector(es->trackId);
  std::vector<unsigned char> is_key_frame = flv_util::to_vector(es->isKeyFrame);
  std::vector<unsigned char> pts = flv_util::to_vector(es->pts);
  std::vector<unsigned char> dts = flv_util::to_vector(es->dts);
  std::vector<unsigned char> size = flv_util::to_vector(es->size);

  std::string es_str(reinterpret_cast<const char*>(es->data->ptr_),es->size);

  if (es_str.empty()) return;

  std::string path =
      "/publish/es/" + context->publish_id_ + "/" + std::to_string(es->frameNumber);

  std::string publish_url = "http://" + context->publish_es_addr_port_ + path;

  castis::http::Request req(publish_url);
  req.SetMethod("POST");
  req.header()["HOST"] = "flv-es-uploader";
  req.header()["User-Agent"] = "flv-es-uploader";
  req.header()["content-type"] = "application/octet-stream";

  req.addBody(track_id);
  req.addBody(is_key_frame);
  req.addBody(pts);
  req.addBody(dts);
  req.addBody(size);
  req.addBody(es_str);

  std::cout << "publish es>" << publish_url << std::endl;
  std::cout << "request string>" << std::endl;
  std::cout << req.header_to_string()<< std::endl;

  std::cout << "request data>" << std::endl;
  std::cout << flv_util::to_hex(es_str)<< std::endl;

	castis::http::Response res;
  if ( client->SendAndReceive(req, &res) == false ) {
    std::cout << "send and receive error" << std::endl;
    ec = 1;
    return ;
  }
  if (res.status_code() < 200 || res.status_code() >= 300) {
    std::cout << "response error[" << res.status_code() <<"]"<< std::endl;
    ec = res.status_code();
    return ;
  }
}


void end_publish(castis::streamer::media_publish_es_context_ptr& context, int&ec)
{
  ec = 0;
  std::string publish_url = "http://" + context->publish_live_addr_port_ +  
    context->publish_live_uri_ + "/" + context->publish_id_;
  castis::http::Request req(publish_url);
  req.SetMethod("DELETE");
  req.header()["HOST"] = "flv-es-uploader";
  req.header()["User-Agent"] = "flv-es-uploader";

  std::cout << "end publish>" << publish_url << std::endl;
  std::cout << "request string>" << std::endl;
  std::cout << req.string() << std::endl;

  castis::http::AsyncClient client;
	castis::http::Response res;

  if ( client.SendAndReceive(req, &res) == false ) {
    std::cout << "send and receive error" << std::endl;
    ec = -1;
    return ;
  }
  if (res.status_code() < 200 || res.status_code() >= 300) {
    std::cout << "response error[" << res.status_code() <<"]"<< std::endl;
    ec = res.status_code();
    return ;
  }  
}

// TODO: 
// async ?
// retry ?
void publish_to_streamer(castis::streamer::media_publish_es_context_ptr context, 
  rtmp_protocol::MediaMessage_ptr request) {
  using namespace castis::streamer;

  unsigned char* data = request->get_data().get();
  std::size_t data_len = request->get_data_len();
  int ec; 

  bool ret = flv_message::process_flv_es_message(
    context, 
    static_cast<uint8_t>(request->get_header()->msg_type_id_),
    static_cast<uint32_t>(request->get_header()->timestamp_),
    static_cast<uint32_t>(request->get_header()->msg_length_),  
    data, data_len, ec);

  RTMPLOGF(debug,"process es. context[%1%],stream_type[%2%],stream_name[%3%],client_id[%4%]",to_string(context), context->stream_type_, context->stream_name_, context->client_id_);

  auto& state = context->publish_state_;
  if (state == MediaPublishEsContext::begin)
  {
    std::string url = "http://" + context->publish_live_addr_port_ + context->publish_live_uri_;
    std::string upload_id, publish_addr;
    int ec;
    begin_publish(url, context, publish_addr, upload_id, ec);
    if (ec!=0) {
      std::cout << "begin publish fail. ec[" << ec << "]" << std::endl;
      return;
    }
    context->publish_id_ = upload_id;
    context->publish_es_addr_port_ = publish_addr;
    state = MediaPublishEsContext::init;

    context->client_ = std::make_shared<castis::http::AsyncClient>();
  }
  {
    if (state == MediaPublishEsContext::init && 
      (ready_to_send(context) || context->ready_to_end_of_stream_)
      
      ) {
        int ec;
        init_publish(context->client_.get(), context, ec);
        if(ec!=0) {
          std::cout << "init publish fail. ec[" << ec << "]" << std::endl;
          return;
        }
        state = MediaPublishEsContext::publishing;
    }

    while(state == MediaPublishEsContext::publishing && not context->media_es_.empty()) {

      std::cout << "media_es_size[" << context->media_es_.size() << "]" << std::endl;
      int ec;
      publish_es(context->client_.get(), context, ec);
      if(ec!=0) {
        std::cout << "publish es fail. ec[" << ec << "]" << std::endl;
        return;
      }        
      context->media_es_.pop_front();
      
    }
  }
  if (context->media_es_.empty() &&
      (context->ready_to_end_of_stream_ || end_of_video_es(context))) {
    int ec;
    end_publish(context, ec);
    if(ec!=0) {
      std::cout << "end publish fail. ec[" << ec << "]" << std::endl;
      return;
    }
    state=MediaPublishEsContext::end;
  }

}

void publish_to_streamer(castis::streamer::media_publish_es_context_ptr context) {
  using namespace castis::streamer;
  RTMPLOGF(debug,"process es. context[%1%],stream_type[%2%],stream_name[%3%],client_id[%4%]",to_string(context), context->stream_type_, context->stream_name_, context->client_id_);

  auto& state = context->publish_state_;
  if (state == MediaPublishEsContext::begin)
  {
    std::string url = "http://" + context->publish_live_addr_port_ + context->publish_live_uri_;
    std::string upload_id, publish_addr;
    int ec;
    begin_publish(url, context, publish_addr, upload_id, ec);
    if (ec!=0) {
      std::cout << "begin publish fail. ec[" << ec << "]" << std::endl;
      return;
    }
    context->publish_id_ = upload_id;
    context->publish_es_addr_port_ = publish_addr;
    state = MediaPublishEsContext::init;

    context->client_ = std::make_shared<castis::http::AsyncClient>();
  }
  {
    if (state == MediaPublishEsContext::init && 
      (ready_to_send(context) || context->ready_to_end_of_stream_)
      
      ) {
        int ec;
        init_publish(context->client_.get(), context, ec);
        if(ec!=0) {
          std::cout << "init publish fail. ec[" << ec << "]" << std::endl;
          return;
        }
        state = MediaPublishEsContext::publishing;
    }

    while(state == MediaPublishEsContext::publishing && not context->media_es_.empty()) {

      std::cout << "media_es_size[" << context->media_es_.size() << "]" << std::endl;
      int ec;
      publish_es(context->client_.get(), context, ec);
      if(ec!=0) {
        std::cout << "publish es fail. ec[" << ec << "]" << std::endl;
        return;
      }        
      context->media_es_.pop_front();
      
    }
  }
  if (context->media_es_.empty() &&
      (context->ready_to_end_of_stream_ || end_of_video_es(context))) {
    int ec;
    end_publish(context, ec);
    if(ec!=0) {
      std::cout << "end publish fail. ec[" << ec << "]" << std::endl;
      return;
    }
    state=MediaPublishEsContext::end;
  }

}

} // namespace rtmp_logic