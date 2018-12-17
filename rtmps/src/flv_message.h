#pragma once
#include "flv_util.h"
#include <vector>
#include <list>
#include <memory>
#include "../../http_async_client/HTTPAsyncClient.h"

namespace castis {
namespace streamer {

typedef enum { video=1, audio=2 } media_es_type_t;
const uint64_t READY_TO_SEND_AUDIO_FRAME_COUNT = 0;
const uint64_t READY_TO_SEND_VIDEO_FRAME_COUNT = 0;

struct AudioPublishEsInit {
public:
  uint8_t trackId{2};
  std::string trackType{"audio"};
  uint32_t timescale{90000};
  std::string codec{"aac"};
  std::vector<unsigned char> codecInfo;
  
  std::string publish_id;
  uint32_t sample_rate; // big endian string
  uint32_t channel_count; // big endian string
};

struct VideoPublishEsInit {
public:
  uint8_t trackId{1};
  std::string trackType{"video"};
  uint32_t timescale{90000};
  std::string codec{"h264"};
  std::vector<unsigned char> codecInfo;
  
  std::string publish_id;
  std::vector<unsigned char> sps;
  std::vector<unsigned char> pps;
};

struct MediaPublishEs {
public:
  media_es_type_t type{video};
  uint64_t frameNumber{0};
  uint8_t trackId{2};
  uint8_t isKeyFrame{1};
  uint64_t pts{0};
  uint64_t dts{0};
  uint32_t size{0};
  
  std::string publish_id;
  flv_util::buffer_t* const data;

  MediaPublishEs(flv_util::buffer_t* const buffer) 
  : data(buffer) {
  }
  ~MediaPublishEs() {
    delete data;
  }
};

typedef std::shared_ptr<MediaPublishEs> media_publish_es_ptr;

struct RecordingFlvContext {
  bool write_audio_header_done{false};
  bool write_video_header_done{false};
  bool write_flv_header_done{false};

  unsigned int audio_index{0};
  unsigned int video_index{0};
  unsigned int media_index{0};

};

struct MediaPublishEsContext {
public: 
  typedef enum { begin, init, publishing, end } publish_state_t;
  publish_state_t publish_state_{begin};

  std::string publish_live_addr_port_{"172.16.33.52:8081"};
  std::string publish_live_uri_{"/cdn/live"};

  std::string publish_id_;
  std::string publish_es_addr_port_;
  AudioPublishEsInit audio_init_es_;
  VideoPublishEsInit video_init_es_;
  std::vector<unsigned char> video_eos_;
  
  uint8_t nal_startcode_len_{4};
  uint32_t video_timestamp_delta_{0};
  uint32_t audio_timestamp_delta_{0};

  std::string provider_{"brokering"};
  std::string stream_name_;
  uint8_t recording_{1};

  std::string stream_type_;
  std::string client_id_;
  unsigned int stream_id_{0};
  bool ready_to_end_of_stream_{false};

  std::list<media_publish_es_ptr> media_es_;
  uint64_t frame_number_{0};
  uint64_t audio_frame_number_{0};
  uint64_t video_frame_number_{0};

  RecordingFlvContext record_flv_context_;
  std::shared_ptr<castis::http::AsyncClient> client_;
};

typedef std::shared_ptr<MediaPublishEsContext> media_publish_es_context_ptr;

std::string to_string(castis::streamer::media_publish_es_context_ptr& context);

bool ready_to_send(castis::streamer::media_publish_es_context_ptr& context);
bool end_of_video_es(castis::streamer::media_publish_es_context_ptr& context);
castis::streamer::media_publish_es_ptr make_media_es(
  media_es_type_t type,
  flv_util::buffer_t* const data,
  uint64_t frame_number,
  uint8_t is_key_frame,
  uint64_t pts,
  uint64_t dts,
  uint32_t size,
  int& ec
);


} // name space streamer
} // name space castis

namespace flv_message {

enum FLV_ES_TYPE {
  FLV_AUDIO_ES = 8,
  FLV_VIDEO_ES = 9
};

bool read_flv_es_dump_file(castis::streamer::media_publish_es_context_ptr& context, std::string const& file_path);
bool process_flv_es_message(
  castis::streamer::media_publish_es_context_ptr& context, 
  uint8_t message_type,
  uint32_t timestamp,
  uint32_t message_length,  
  unsigned char* const buffer, 
  std::size_t const buffer_size,
  int& ec);
void read_flv_es_dump(castis::streamer::media_publish_es_context_ptr& context, unsigned int start, unsigned int end);
bool process_flv_es_dump_message(
  castis::streamer::media_publish_es_context_ptr& context, 
  unsigned char* const buffer, 
  std::size_t const buffer_size,
  int& ec);



} // namespace flv_message
