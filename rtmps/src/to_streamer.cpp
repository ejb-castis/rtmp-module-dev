#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "streamer/base64.h"
#include "streamer/session.h"

#include "flv_message.h"
#include "publish_to_streamer.h"
#include "rtmpingest.h"
#include "rtmpmodulelogger.h"

namespace fs = boost::filesystem;
namespace asio = boost::asio;
namespace pt = boost::posix_time;
using namespace castis::streamer;

namespace rtmp_logic {

void process_rtmp_message(
    castis::streamer::media_publish_es_context_ptr context,
    rtmp_protocol::MediaMessage_ptr request) {
  unsigned char* data = request->get_data().get();
  std::size_t data_len = request->get_data_len();
  int ec = 0;

  bool ret = flv_message::process_flv_es_message(
      context, static_cast<uint8_t>(request->get_header()->msg_type_id_),
      static_cast<uint32_t>(request->get_header()->abs_timestamp_),
      static_cast<uint32_t>(request->get_header()->msg_length_), data, data_len,
      ec);

  if (!ret) {
    RTMPLOGF(error,
             "process rtmp flv message failed. dropped message,"
             "stream_type[%1%],stream_name[%2%],client_id[%3%],context[%4%]",
             context->stream_type_, context->stream_name_, context->client_id_,
             to_string(context));
    return;
  }

  RTMPLOGF(debug,
           "process es. "
           "stream_type[%1%],stream_name[%2%],client_id[%3%],context[%4%]",
           context->stream_type_, context->stream_name_, context->client_id_,
           to_string(context));
  publish_to_streamer(context);
}

bool start_ingest(castis::streamer::media_publish_es_context_ptr context) {
  RTMPLOGF(debug,
           "start ingest. "
           "stream_type[%1%],stream_name[%2%],client_id[%3%],context[%4%]",
           context->stream_type_, context->stream_name_, context->client_id_,
           to_string(context));
  auto mi =
      std::make_shared<Ingest>(context->ioc_, context->api_, context->config_);

  bool recording = (context->recording_ == 1) ? true : false;
  auto st = mi->create(context->stream_name_, context->provider_, recording);
  if (st.ok() == false) {
    RTMPLOGF(error,
             "fails to start ingest. "
             "%1%, "
             "stream_type[%2%],stream_name[%3%],client_id[%4%],context[%5%]",
             st.error(), context->stream_type_, context->stream_name_,
             context->client_id_, to_string(context));
    to_string(context);
    return false;
  }
  context->ingest_ = mi;
  context->publish_id_ = context->stream_name_;
  context->api_->service_->ingest_->AddIngest(mi);
  return true;
}

bool init_es_ingest2(castis::streamer::media_publish_es_context_ptr context) {
  SetIngestProcessing(context->ingest_);
  RTMPLOGF(debug,
           "init es ingest. "
           "stream_type[%1%],stream_name[%2%],client_id[%3%],context[%4%]",
           context->stream_type_, context->stream_name_, context->client_id_,
           to_string(context));

  auto vbuf =
      reinterpret_cast<const char*>(context->video_init_es_.codecInfo.data());
  std::string avc_codec_info(vbuf, context->video_init_es_.codecInfo.size());
  auto abuf =
      reinterpret_cast<const char*>(context->audio_init_es_.codecInfo.data());
  std::string aac_codec_info(abuf, context->audio_init_es_.codecInfo.size());

  EsTrackInfo avctrack;
  avctrack.id_ = 1;
  avctrack.type_ = "video";
  avctrack.timescale_ = 90000;
  avctrack.codec_ = "h264";
  avctrack.codec_info_ = avc_codec_info;

  EsTrackInfo aactrack;
  aactrack.id_ = 2;
  aactrack.type_ = "audio";
  aactrack.timescale_ = 90000;
  aactrack.codec_ = "aac";
  aactrack.codec_info_ = aac_codec_info;

  std::vector<EsTrackInfo> tracks;
  tracks.push_back(avctrack);
  tracks.push_back(aactrack);

  auto st = context->ingest_->es_init_segment(tracks);
  if (st.ok() == false) {
    RTMPLOGF(error,
             "fails to init ingest segment. "
             "%1%, "
             "stream_type[%2%],stream_name[%3%],client_id[%4%],context[%5%]",
             st.error(), context->stream_type_, context->stream_name_,
             context->client_id_, to_string(context));
    return false;
  }
  return true;
}

bool init_es_ingest(castis::streamer::media_publish_es_context_ptr context) {
  return init_es_ingest2(context);

  SetIngestProcessing(context->ingest_);
  RTMPLOGF(debug,
           "init es ingest. "
           "stream_type[%1%],stream_name[%2%],client_id[%3%],context[%4%]",
           context->stream_type_, context->stream_name_, context->client_id_,
           to_string(context));
  std::string avc;
  {
    auto buf =
        reinterpret_cast<const char*>(context->video_init_es_.codecInfo.data());
    std::string avc_data(buf, context->video_init_es_.codecInfo.size());
    if (Base64::Encode(avc_data, &avc) == false) {
      RTMPLOGF(error,
               "fails to init es ingest. fails to encode base64 with avc codec "
               "info. "
               "stream_type[%1%],stream_name[%2%],client_id[%3%],context[%4%]",
               context->stream_type_, context->stream_name_,
               context->client_id_, to_string(context));
      return false;
    }
  }
  std::string aac;
  {
    auto buf =
        reinterpret_cast<const char*>(context->audio_init_es_.codecInfo.data());
    std::string aac_data(buf, context->audio_init_es_.codecInfo.size());
    if (Base64::Encode(aac_data, &aac) == false) {
      RTMPLOGF(error,
               "fails to init es ingest. fails to encode base64 with aac codec "
               "info. "
               "stream_type[%1%],stream_name[%2%],client_id[%3%],context[%4%]",
               context->stream_type_, context->stream_name_,
               context->client_id_, to_string(context));
      return false;
    }
  }

  std::string f = R"_({
    "tracks" : [
      {"trackId" : 1, "trackType" : "video", "timescale" : 90000, "codec": "h264", "codecInfo" : "%s"},
      {"trackId" : 2, "trackType" : "audio", "timescale" : 90000, "codec": "aac", "codecInfo" : "%s"}
    ]
  })_";
  std::string json = (boost::format(f) % avc.c_str() % aac.c_str()).str();

  auto st = context->ingest_->OnEsInitSegment(json);
  if (st.ok() == false) {
    RTMPLOGF(error,
             "fails to init ingest segment. "
             "%1%, "
             "stream_type[%2%],stream_name[%3%],client_id[%4%],context[%5%]",
             st.error(), context->stream_type_, context->stream_name_,
             context->client_id_, to_string(context));
    return false;
  }
  return true;
}
bool do_es_ingest(castis::streamer::media_publish_es_context_ptr context) {
  if (context->media_es_.empty()) return true;

  SetIngestProcessing(context->ingest_);
  RTMPLOGF(debug,
           "do es ingest. "
           "stream_type[%1%],stream_name[%2%],client_id[%3%],context[%4%]",
           context->stream_type_, context->stream_name_, context->client_id_,
           to_string(context));

  auto es = context->media_es_.front().get();
  std::string segment = std::to_string(es->frameNumber);
  auto st = context->ingest_->OnSegment(segment);
  if (st.ok() == false) {
    RTMPLOGF(error,
             "fails to ingest segment. "
             "%1%, "
             "stream_type[%2%],stream_name[%3%],client_id[%4%],context[%5%]",
             st.error(), context->stream_type_, context->stream_name_,
             context->client_id_, to_string(context));
    return false;
  }

  std::vector<unsigned char> track_id = flv_util::to_vector(es->trackId);
  std::vector<unsigned char> is_key_frame = flv_util::to_vector(es->isKeyFrame);
  std::vector<unsigned char> pts = flv_util::to_vector(es->pts);
  std::vector<unsigned char> dts = flv_util::to_vector(es->dts);
  std::vector<unsigned char> size = flv_util::to_vector(es->size);
  std::string es_data(reinterpret_cast<const char*>(es->data->ptr_), es->size);

  std::string chunk;
  std::copy(track_id.begin(), track_id.end(), std::back_inserter(chunk));
  std::copy(is_key_frame.begin(), is_key_frame.end(),
            std::back_inserter(chunk));
  std::copy(pts.begin(), pts.end(), std::back_inserter(chunk));
  std::copy(dts.begin(), dts.end(), std::back_inserter(chunk));
  std::copy(size.begin(), size.end(), std::back_inserter(chunk));
  std::copy(es_data.begin(), es_data.end(), std::back_inserter(chunk));

  // es->headersize(=1+1+8+8+4) + es->size
  std::size_t segment_size = chunk.size();

  st = context->ingest_->OnEsChunk(segment, chunk);
  if (st.ok() == false) {
    RTMPLOGF(error,
             "fails to ingest segment. "
             "%1%, "
             "stream_type[%2%],stream_name[%3%],client_id[%4%],context[%5%]",
             st.error(), context->stream_type_, context->stream_name_,
             context->client_id_, to_string(context));

    return false;
  }
  context->ingest_->last_segment_ = segment;
  context->ingest_->last_segment_size_ = segment_size;
  context->ingest_->last_processed_ = pt::microsec_clock::local_time();

  return true;
}
bool stop_ingest(castis::streamer::media_publish_es_context_ptr context) {
  RTMPLOGF(debug,
           "stop ingest. "
           "stream_type[%1%],stream_name[%2%],client_id[%3%],context[%4%]",
           context->stream_type_, context->stream_name_, context->client_id_,
           to_string(context));
  asio::spawn(context->strand_, [=](boost::asio::yield_context yield) mutable {
    context->api_->service_->ingest_->DeleteIngest(context->ingest_->id_);
    auto st = context->ingest_->OnStop(yield);
    if (st.ok()) {
      RTMPLOGF(debug,
               "ingest stopped. "
               "%1%, "
               "stream_type[%2%],stream_name[%3%],client_id[%4%],context[%5%]",
               st.error(), context->stream_type_, context->stream_name_,
               context->client_id_, to_string(context));
      return true;
    } else {
      RTMPLOGF(error,
               "fails to stop ingest. "
               "%1%, "
               "stream_type[%2%],stream_name[%3%],client_id[%4%],context[%5%]",
               st.error(), context->stream_type_, context->stream_name_,
               context->client_id_, to_string(context));

      return false;
    }
  });
  return true;
}

void publish_to_streamer(
    castis::streamer::media_publish_es_context_ptr context) {
  auto& state = context->publish_state_;
  if (state == MediaPublishEsContext::begin) {
    if (!start_ingest(context)) {
      return;
    }
    state = MediaPublishEsContext::init;
  }

  if (state == MediaPublishEsContext::init && ready_to_send(context)) {
    if (!init_es_ingest(context)) {
      return;
    }
    state = MediaPublishEsContext::publishing;
  }

  while (state == MediaPublishEsContext::publishing &&
         not context->media_es_.empty()) {
    if (!do_es_ingest(context)) {
      return;
    }
    context->media_es_.pop_front();
  }

  if (state != MediaPublishEsContext::end &&
      (context->ready_to_end_of_stream_ || end_of_video_es(context))) {
    if (!stop_ingest(context)) {
      return;
    } else {
      state = MediaPublishEsContext::end;
      return;
    }
  }
}

}  // namespace rtmp_logic