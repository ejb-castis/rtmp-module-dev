#include <string>
#include <iostream>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <memory>

#include "flv_util.h"
#include "parsingflv.h"

#include "flv_message.h"
#include "streamer/base64.h"
#include "HTTPAsyncClient.h"

using namespace castis::streamer;
using namespace flv;
using namespace parsingflv;
using namespace flv_util;

namespace castis {
namespace streamer {


std::string to_string(MediaPublishEs* es)
{
  std::vector<std::string> media_type{ "unknown", "video", "audio" };

  std::ostringstream oss;
  oss << "frameNumber[" << es->frameNumber <<"],";
  oss << "type[" << media_type.at(static_cast<uint16_t>(es->type)) <<"],";
  oss << "isKeyFrame[" << static_cast<uint16_t>(es->isKeyFrame) <<"],";
  oss << "pts[" << es->pts <<"],";
  oss << "dts[" << es->dts <<"],";
  oss << "size[" << es->size <<"],";
  oss << std::endl << "media_data[" << to_hex(*es->data) <<"]";
  return oss.str();  
}

std::string to_string(media_publish_es_context_ptr& context)
{
  std::vector<std::string> publish_state{ "begin", "init", "publishing", "end" };

  std::ostringstream oss;
  oss << "publish_id[" << context->publish_id_ << "],"; 
  oss << "publish_state[" << publish_state.at(static_cast<uint16_t>(context->publish_state_)) << "],"; 
  oss << "frame_number[" << context->frame_number_ << "],"; 
  oss << "video_frame_number[" << context->video_frame_number_ << "],"; 
  oss << "audio_frame_number[" << context->audio_frame_number_ << "],"; 
  if (context->media_es_.back().get()) {
    oss << "last es_frame_data[" << to_string(context->media_es_.back().get()) << "]";
  } else {
    oss << "no es_frame_data" ; 
  }


  return oss.str();  
}

bool ready_to_send(media_publish_es_context_ptr& context) {
  if (not context->audio_init_es_.codecInfo.empty() and
    not context->video_init_es_.codecInfo.empty() and
    context->video_frame_number_ >= READY_TO_SEND_VIDEO_FRAME_COUNT and
    context->audio_frame_number_ >= READY_TO_SEND_AUDIO_FRAME_COUNT ) {
    return true;
  }

  return false;  
}

bool end_of_video_es(media_publish_es_context_ptr& context) {
  if (not context->video_eos_.empty()) {
    return true;
  }
  return false;  
}

media_publish_es_ptr make_media_es(
  media_es_type_t type,
  flv_util::buffer_t* const data,
  uint64_t frame_number,
  uint8_t is_key_frame,
  uint64_t pts,
  uint64_t dts,
  uint32_t size,
  int& ec
) {
  ec= 0;
  if (type == video) {
    
    media_publish_es_ptr es = std::make_shared<MediaPublishEs>(data);  
    es->type = video;
    es->frameNumber = frame_number;
    es->trackId = static_cast<uint8_t>(video);
    es->pts = pts;
    es->dts = dts;
    es->size = size;

    return es;
  }
  else if (type == audio) {

    media_publish_es_ptr es = std::make_shared<MediaPublishEs>(data);  
    es->type = audio;
    es->frameNumber = frame_number;
    es->trackId = static_cast<uint8_t>(audio);
    es->pts = pts;
    es->dts = dts;
    es->size = size;

    return es;
  }

  ec = 1;
  return media_publish_es_ptr(nullptr);
}


} // namespace castis
} // namespace streamer


namespace flv_message {


bool process_flv_es_message(
  media_publish_es_context_ptr& context, 
  uint8_t message_type,
  uint32_t timestamp,
  uint32_t message_length,  
  unsigned char* const buffer, 
  std::size_t const buffer_size,
  int& ec) {

  ec=0;    
  unsigned char* bf = buffer;
  std::size_t len = buffer_size;

  if (message_type == FLV_AUDIO_ES) {
    if (is_aac_sequence_header(bf)) {
      uint32_t sample_rate;
      uint32_t channel_count;
      if (get_aac_header(bf, sample_rate, channel_count, len)) {
        context->audio_init_es_.sample_rate = sample_rate;
        context->audio_init_es_.channel_count = channel_count;
        
        std::vector<unsigned char> codec_info;
        std::vector<unsigned char> sv = to_vector(sample_rate);
        std::vector<unsigned char> cv = to_vector(channel_count);
        
        codec_info.insert(codec_info.begin(), cv.begin(), cv.end());
        codec_info.insert(codec_info.begin(), sv.begin(), sv.end());
        context->audio_init_es_.codecInfo = codec_info;

        //std::cout << "aac_sequence_header parsed" << std::endl;
      } else {
        ec = 1;
        std::cout << "parsing aac_sequence_header fail" << std::endl;
      }
    } else if (is_aac_audio(bf)) {
      buffer_t* audio_es = new buffer_t(len);
      flv::audio_frame_t frame;
      uint64_t dts = timestamp * 90;
      uint64_t pts = dts;
      if (get_audio(bf, *audio_es, frame, len)) {
        media_publish_es_ptr media_es =
        make_media_es(castis::streamer::audio, 
        audio_es,
        context->frame_number_ + 1,
        1, pts, dts, 
        audio_es->len_, ec);
        
        context->media_es_.push_back(media_es);
        ++context->frame_number_;
        ++context->audio_frame_number_;

        //std::cout << "aac_data parsed" << std::endl;
      }
      else {
        ec = 1;
        std::cout << "getting aac_data fail" << std::endl;
        delete audio_es;
      }
    } else {
      ec = 1;
      std::cout << "parsing audio data fail" << std::endl;
      std::cout << "message_type[" << to_hex(message_type) <<"],";
      std::cout << "timestamp[" << timestamp <<"],";
      std::cout << "message_length[" << message_length <<"]]" << std::endl;
      std::cout << "message dump > " << to_hex(buffer, buffer_size) << std::endl;;      
    }
  }
  else if (message_type == FLV_VIDEO_ES) {
    if(is_avc_sequence_header(bf)) {
      buffer_t sps(0xFFFF);
      buffer_t pps(0xFFFF);
      uint8_t nal_startcode_len;
      if (get_first_sps_and_first_pps_from_avc_sequence_header(
        bf, sps, pps, nal_startcode_len, len)) {
        context->video_init_es_.sps = to_vector(sps);
        context->video_init_es_.pps = to_vector(pps);
        context->nal_startcode_len_ = nal_startcode_len;

        std::vector<unsigned char> codec_info;
        
        unsigned char d[] = { 0x00,0x00,0x00,0x01 };
        codec_info.insert (codec_info.begin(), context->video_init_es_.pps.begin(), context->video_init_es_.pps.end());
        codec_info.insert (codec_info.begin(), d, d+4);
        codec_info.insert (codec_info.begin(), context->video_init_es_.sps.begin(), context->video_init_es_.sps.end());
        codec_info.insert (codec_info.begin(), d, d+4);

        context->video_init_es_.codecInfo = codec_info;

        //std::cout << "avc_sequence_header parsed" << std::endl;
      } else {
        ec = 1;
        std::cout << "parsing avc_sequence_header fail" << std::endl;
      }     
    } else if (is_avc_video(bf)) {
      buffer_t* video_es = new buffer_t(len);
      flv::video_frame_t frame;
      uint64_t dts = timestamp * 90;
      uint64_t pts = dts + frame.cts_ * 90 ;
      uint8_t key = frame.keyframe_;

      if (get_video(bf, *video_es, frame, len)) {
        parsingflv::replace_nalu_start_code_from_mp4_to_ts(4, *video_es);

        media_publish_es_ptr media_es =
        make_media_es(castis::streamer::video, 
        video_es,
        context->frame_number_ + 1,
        key, pts, dts, 
        video_es->len_, ec);

        context->media_es_.push_back(media_es);
        ++context->frame_number_;
        ++context->video_frame_number_;

        //std::cout << "avc_video parsed" << std::endl;
      } else {
        ec = 1;
        std::cout << "parsing avc_video fail" << std::endl;
        delete video_es;
      }
    } else if (is_avc_end_of_sequence(bf)) {
      buffer_t eos(len);
      get_avc_end_of_sequence(bf, eos, len);
      context->video_eos_ = to_vector(eos);
    } else {
      ec = 1;
      std::cout << "parsing video data fail" << std::endl;
      std::cout << "message_type[" << to_hex(message_type) <<"],";
      std::cout << "timestamp[" << timestamp <<"],";
      std::cout << "message_length[" << message_length <<"]]" << std::endl;
      std::cout << "message dump > " << to_hex(buffer, buffer_size) << std::endl;;      
    }
  } else {
    ec = 1;
    std::cout << "unknown message type["; 
    std::cout << "message_type[" << to_hex(message_type) <<"],";
    std::cout << "timestamp[" << timestamp <<"],";
    std::cout << "message_length[" << message_length <<"]]" << std::endl;
    std::cout << "message dump > " << to_hex(buffer, buffer_size) << std::endl;;
  }
 
  return true;
}



bool read_flv_es_dump_file(media_publish_es_context_ptr& context, std::string const& file_path) {

  std::cout << "open >" << file_path << std::endl;

  std::ifstream binfile(file_path, std::ios::binary);
  if (not binfile) { std::cout << "> open false" << std::endl; return false; }

  // get length of file:
  binfile.seekg(0, binfile.end);
  std::size_t length = binfile.tellg();
  binfile.seekg(0, binfile.beg);
  std::cout << "> file length[" << length << "]" << std::endl;
  std::size_t const buffer_size = length;
  unsigned char* buffer = new unsigned char[buffer_size];

  binfile.read(reinterpret_cast<char*>(buffer), buffer_size);
  if (not binfile) { std::cout << "> read false" << std::endl; return false; }

  int ec; 
  bool ret = process_flv_es_dump_message(context, buffer, buffer_size, ec);

  std::cout << "process_flv_es_message. ec[" << ec << "]" << ",ret[" << ret << "],";
  std::cout << "context[" << to_string(context) << "]" ;
  std::cout << std::endl;


  binfile.close();
  delete[] buffer;
  return true;
}



bool process_flv_es_dump_message(
  media_publish_es_context_ptr& context, 
  unsigned char* const buffer, 
  std::size_t const buffer_size,
  int& ec) {

  ec=0;    
  unsigned char* bf = buffer;
  std::size_t len = buffer_size;

  uint8_t message_type{0};
  uint32_t timestamp{0};
  uint32_t message_length{0};
  read_1byte(bf, message_type, len);
  read_4byte(bf, timestamp, len);
  read_4byte(bf, message_length, len);

  if (message_type == FLV_AUDIO_ES) {
    if (is_aac_sequence_header(bf)) {
      uint32_t sample_rate;
      uint32_t channel_count;
      if (get_aac_header(bf, sample_rate, channel_count, len)) {
        context->audio_init_es_.sample_rate = sample_rate;
        context->audio_init_es_.channel_count = channel_count;
        
        std::vector<unsigned char> codec_info;
        std::vector<unsigned char> sv = to_vector(sample_rate);
        std::vector<unsigned char> cv = to_vector(channel_count);
        
        codec_info.insert(codec_info.begin(), cv.begin(), cv.end());
        codec_info.insert(codec_info.begin(), sv.begin(), sv.end());
        context->audio_init_es_.codecInfo = codec_info;

        //std::cout << "aac_sequence_header parsed" << std::endl;
      } else {
        ec = 1;
        std::cout << "parsing aac_sequence_header fail" << std::endl;
      }
    } else if (is_aac_audio(bf)) {
      buffer_t* audio_es = new buffer_t(len);
      flv::audio_frame_t frame;
      uint64_t dts = timestamp * 90;
      uint64_t pts = dts;
      if (get_audio(bf, *audio_es, frame, len)) {
        media_publish_es_ptr media_es =
        make_media_es(castis::streamer::audio, 
        audio_es,
        context->frame_number_ + 1,
        1, pts, dts, 
        audio_es->len_, ec);
        
        context->media_es_.push_back(media_es);
        ++context->frame_number_;
        ++context->audio_frame_number_;

        //std::cout << "aac_data parsed" << std::endl;
      }
      else {
        ec = 1;
        std::cout << "getting aac_data fail" << std::endl;
        delete audio_es;
      }
    } else {
      ec = 1;
      std::cout << "parsing audio data fail" << std::endl;
      std::cout << "message_type[" << to_hex(message_type) <<"],";
      std::cout << "timestamp[" << timestamp <<"],";
      std::cout << "message_length[" << message_length <<"]]" << std::endl;
      std::cout << "message dump > " << to_hex(buffer, buffer_size) << std::endl;;      
    }
  }
  else if (message_type == FLV_VIDEO_ES) {
    if(is_avc_sequence_header(bf)) {
      buffer_t sps(0xFFFF);
      buffer_t pps(0xFFFF);
      uint8_t nal_startcode_len;
      if (get_first_sps_and_first_pps_from_avc_sequence_header(
        bf, sps, pps, nal_startcode_len, len)) {
        context->video_init_es_.sps = to_vector(sps);
        context->video_init_es_.pps = to_vector(pps);
        context->nal_startcode_len_ = nal_startcode_len;

        std::vector<unsigned char> codec_info;
        
        unsigned char d[] = { 0x00,0x00,0x00,0x01 };
        codec_info.insert (codec_info.begin(), context->video_init_es_.pps.begin(), context->video_init_es_.pps.end());
        codec_info.insert (codec_info.begin(), d, d+4);
        codec_info.insert (codec_info.begin(), context->video_init_es_.sps.begin(), context->video_init_es_.sps.end());
        codec_info.insert (codec_info.begin(), d, d+4);

        context->video_init_es_.codecInfo = codec_info;

        //std::cout << "avc_sequence_header parsed" << std::endl;
      } else {
        ec = 1;
        std::cout << "parsing avc_sequence_header fail" << std::endl;
      }     
    } else if (is_avc_video(bf)) {
      buffer_t* video_es = new buffer_t(len);
      flv::video_frame_t frame;
      uint64_t dts = timestamp * 90;
      uint64_t pts = dts + frame.cts_ * 90 ;
      uint8_t key = frame.keyframe_;

      if (get_video(bf, *video_es, frame, len)) {
        parsingflv::replace_nalu_start_code_from_mp4_to_ts(4, *video_es);

        media_publish_es_ptr media_es =
        make_media_es(castis::streamer::video, 
        video_es,
        context->frame_number_ + 1,
        key, pts, dts, 
        video_es->len_, ec);

        context->media_es_.push_back(media_es);
        ++context->frame_number_;
        ++context->video_frame_number_;

        //std::cout << "avc_video parsed" << std::endl;
      } else {
        ec = 1;
        std::cout << "parsing avc_video fail" << std::endl;
        delete video_es;
      }
    } else if (is_avc_end_of_sequence(bf)) {
      buffer_t eos(len);
      get_avc_end_of_sequence(bf, eos, len);
      context->video_eos_ = to_vector(eos);
    } else {
      ec = 1;
      std::cout << "parsing video data fail" << std::endl;
      std::cout << "message_type[" << to_hex(message_type) <<"],";
      std::cout << "timestamp[" << timestamp <<"],";
      std::cout << "message_length[" << message_length <<"]]" << std::endl;
      std::cout << "message dump > " << to_hex(buffer, buffer_size) << std::endl;;      
    }
  } else {
    ec = 1;
    std::cout << "unknown message type["; 
    std::cout << "message_type[" << to_hex(message_type) <<"],";
    std::cout << "timestamp[" << timestamp <<"],";
    std::cout << "message_length[" << message_length <<"]]" << std::endl;
    std::cout << "message dump > " << to_hex(buffer, buffer_size) << std::endl;;
  }
 
  return true;
}


void read_flv_es_dump(media_publish_es_context_ptr& context, unsigned int start, unsigned int end) {

  std::string directory = "/data/project/git/rtmp-module/build/dump/flv_es";
  std::string file_name = "flv_es.dump";
  std::string file_path;

  for (unsigned index=start; index <= end; ++index )  {
    file_path = directory + "/" + file_name + "." + std::to_string(index);

    read_flv_es_dump_file(context, file_path);

  }
}

} // namespace flv_message
