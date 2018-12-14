#pragma once
#include <string>
#include "flv_util.h"

namespace flv {

// frame.dts = flv_video_frame_t.timestamp_ * 90;
// frame.pts = frame.dts + flv_video_frame_t.cts_ * 90;
// frame.pid = 0x100;
// frame.sid = 0xe0;
// frame.keyframe = flv_video_frame_t.keyframe_;
struct video_frame_t {
public:
  uint64_t timestamp_{0};
  uint64_t cts_{0};
  unsigned char keyframe_{1};
  unsigned char codec_id_{7}; // h.264
};

// audio.pts = timestamp * 90 
struct audio_frame_t {
public:
  uint64_t timestamp_{0};
  uint32_t sample_rate_{0};
  uint32_t channel_count_{0};
  unsigned char codec_id_{0}; // aac
};

}

namespace parsingflv {

bool is_aac_audio(unsigned char* const & buffer);
bool is_aac_sequence_header(unsigned char* const & buffer);
bool get_aac_header(unsigned char*& buffer, 
  uint32_t& sample_rate, 
  uint32_t& channel_count, 
  std::size_t& buffer_len);
bool get_audio(unsigned char*& buffer, 
  flv_util::buffer_t& audio_es, 
  flv::audio_frame_t& audio_frame,
  std::size_t& buffer_len);
bool is_avc_sequence_header(unsigned char* const & buffer);
bool is_avc_video(unsigned char* const & buffer);  
bool is_avc_end_of_sequence(unsigned char* const & buffer);
std::string encoding_base_64(flv_util::buffer_t &src);
bool get_first_sps_and_first_pps_from_avc_sequence_header(
  unsigned char*& buffer,
  flv_util::buffer_t& sps, flv_util::buffer_t& pps, 
  uint8_t& nal_startcode_len, 
  std::size_t& buffer_len);
bool get_video(unsigned char*& buffer, 
  flv_util::buffer_t& video_es, flv::video_frame_t& video_frame, 
  std::size_t& buffer_len);  
void get_avc_end_of_sequence(unsigned char*& buffer, flv_util::buffer_t& eos, std::size_t& buffer_len);
void replace_nalu_start_code_from_mp4_to_ts(
  unsigned char nal_start_code_length,
  flv_util::buffer_t const& mp4_video_es);
bool parsing_flv(std::string const& file_path);

} // namespace parsingflv