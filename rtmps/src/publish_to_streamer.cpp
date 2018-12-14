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


// #include "rapidjson/document.h"
// #include "rapidjson/error/en.h"
// #include "rapidjson/stringbuffer.h"
// #include "rapidjson/writer.h"
// #include "streamer/base64.h"


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

void init_publish(std::string const& publish_addr, std::string const& upload_id, castis::streamer::media_publish_es_context_ptr& context, std::error_code& ec)
{
  std::string publish_url = "http://" + publish_addr + "/publish/es/" + upload_id + "/init";
  
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
      ec = std::make_error_code(std::errc::invalid_argument);
      std::cout << "failed to encode base64 with avc codec info" << std::endl;
    }
  }

  {
    auto buf = reinterpret_cast<const char*>(context->audio_init_es_.codecInfo.data());
    std::string aac_data(buf, context->audio_init_es_.codecInfo.size());
    if (Base64::Encode(aac_data, &aac) == false) {
      ec = std::make_error_code(std::errc::invalid_argument);
      std::cout << "failed to encode base64 with avc codec info" << std::endl;
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

  castis::http::AsyncClient client;
	castis::http::Response res;

  if ( client.SendAndReceive(req, &res) == false ) {
    std::cout << "send and receive error" << std::endl;
    return ;
  }
  if (res.status_code() < 200 || res.status_code() >= 300) {
    std::cout << "response error[" << res.status_code() <<"]"<< std::endl;
    return ;
  }
 
}

void publish_es(castis::http::AsyncClient& client, std::string const& publish_addr, std::string const& upload_id, castis::streamer::media_publish_es_context_ptr& context, std::error_code& ec)
{
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
      "/publish/es/" + upload_id + "/" + std::to_string(es->frameNumber);

  std::string publish_url = "http://" + publish_addr + path;

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

  if ( client.SendAndReceive(req, &res) == false ) {
    std::cout << "send and receive error" << std::endl;
    return ;
  }
  if (res.status_code() < 200 || res.status_code() >= 300) {
    std::cout << "response error[" << res.status_code() <<"]"<< std::endl;
    return ;
  }

}


void end_publish(std::string const& publish_addr, std::string const& upload_id, int&ec)
{
  ec = 0;
  std::string publish_url = "http://" + publish_addr + "/cdn/live/" + upload_id;
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

void publish_to_streamer(castis::streamer::media_publish_es_context_ptr context, 
  rtmp_protocol::MediaMessage_ptr& request) {
   
  RTMPLOGF(debug,"pulishing stream_name[%1%],client_id[%2%]",context->stream_name_, context->client_id_);

  return;


  typedef enum { begin, init, publishing, end } publish_state_t;

  publish_state_t state = begin;
  std::string url = "http://172.16.33.52:8081/cdn/live";
  std::string upload_id, publish_addr;
  context->provider_ = "brokering";
  context->stream_name_ = "750k.mp4";
  context->recording_ = 1;

  {
    int ec{0};
    begin_publish(url, context, publish_addr, upload_id, ec);
    context->publish_id_ = upload_id;
    context->publish_addr_port_ = publish_addr;
    state = init;
  }

  {  
    using namespace flv_message;
    context->publish_id_ = upload_id;
    unsigned six = 1;
    //unsigned eix = 234; for 20181130104010.mp4
    unsigned eix = 42274; //42274 data/750k.mp4

    std::string directory = "/data/project/git/rtmp-module/build/dump/flv_es";
    std::string file_name = "flv_es.dump";
    std::string file_path;

    castis::http::AsyncClient client;

    for (unsigned index=six; index <= eix; ++index )  {
      file_path = directory + "/" + file_name + "." + std::to_string(index);

      read_flv_es_dump_file(context, file_path);

      if (state == init && ready_to_send(context)) {
        std::error_code ec;
        init_publish(publish_addr, upload_id, context, ec);

        state = publishing;
      }  else if (state == publishing && not context->media_es_.empty()) {

        std::cout << "media_es_size[" << context->media_es_.size() << "]" << std::endl;

        std::error_code ec;
        publish_es(client, publish_addr, upload_id, context, ec);
        std::cout << std::endl;

        context->media_es_.pop_front();
      }
    }

    if (state == publishing ) {
      std::size_t n = context->media_es_.size();
      for (std::size_t i=0 ; i < n; ++i ) {

        std::cout << "media_es_size[" << context->media_es_.size() << "]" << std::endl;

        std::error_code ec;
        publish_es(client, publish_addr, upload_id, context, ec);
        std::cout << std::endl;

        context->media_es_.pop_front();
      }
    }

    state = end;
  }
  if (state==end && end_of_video_es(context))
  {
    int ec{0};
    end_publish(publish_addr, upload_id, ec);
  }





}

} // namespace rtmp_logic