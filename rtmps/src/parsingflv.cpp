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
#include "streamer/base64.h"

using namespace flv_util;
using namespace flv;

namespace parsingflv {

enum FLV_FRAMETYPE {
  FLV_KEY_FRAME = 1,
  FLV_INNER_FRAME =2
};
enum FLV_CODECID {
  FLV_AVC = 7
};
enum FLV_AVC_PACKETTYPE {
  FLV_AVC_SEQUENCE_HEADER = 0,
  FLV_AVC_NALU=1,
  FLV_AVC_EOS=2
};
enum FLV_SOUND_FORMAT {
  FLV_AAC = 10
};
enum FLV_SOUND_RATE {
  FLV_5_5kHZ = 0,
  FLV_11kHZ = 1,
  FLV_22kHz = 2,
  FLV_44kHz = 3
};
enum FLV_AAC_PACKETTYPE {
  FLV_AAC_SEQUENCE_HEADER = 0,
  FLV_AAC_RAW = 1,
};

std::string to_string(video_frame_t& frame) {
  std::ostringstream oss;
  oss << "timestamp[" << frame.timestamp_ << "],";
  oss << "cts[" <<  frame.cts_ << "],";
  oss << "keyframe[" <<  frame.keyframe_ << "],";
  oss << "codec_id[" <<  frame.codec_id_ << "]";
  return oss.str();
}
std::string to_string(audio_frame_t& frame) {
  std::ostringstream oss;
  oss << "timestamp[" << frame.timestamp_ << "],";
  oss << "sample_rate[" <<  frame.sample_rate_ << "],";
  oss << "channel_count[" <<  frame.channel_count_ << "],";
  oss << "codec_id[" <<  frame.codec_id_ << "]";
  return oss.str();
}

bool is_aac_audio(unsigned char* const & buffer) {
  uint16_t fmt_aac_type;
  get_2byte(buffer, fmt_aac_type); 
  std::cout << "fmt_aac_type[" << to_hex(fmt_aac_type) << "]->";
    
  unsigned char fmt = (fmt_aac_type & 0xff00) >> 8;
  std::cout << "fmt[" << to_hex(fmt) << "],";

  unsigned char sound_format = (fmt & 0xf0) >> 4;
  std::cout << "sound_format[" << to_hex(sound_format) << "]" << std::endl;

  unsigned char aac_packet_type = fmt_aac_type & 0x00ff;
  std::cout << "aac_packet_type[" << to_hex(aac_packet_type) << "]" << std::endl;

  if (sound_format == FLV_AAC && 
    aac_packet_type == FLV_AAC_RAW) {
    return true;
  }

  return false;
}

bool is_aac_sequence_header(unsigned char* const & buffer) {
  uint16_t fmt_aac_type;
  get_2byte(buffer, fmt_aac_type); 
  std::cout << "fmt_aac_type[" << to_hex(fmt_aac_type) << "]->";

  unsigned char fmt = (fmt_aac_type & 0xff00) >> 8;
  std::cout << "fmt[" << to_hex(fmt) << "],";

  unsigned char sound_format = (fmt & 0xf0) >> 4;
  std::cout << "sound_format[" << to_hex(sound_format) << "], ";

  unsigned char sound_rate = fmt & 0x0c >> 2;
  std::cout << "sound_rate[" << to_hex(sound_rate) << "],";

  unsigned char sound_size = fmt & 0x02 >> 1;
  std::cout << "sound_size[" << to_hex(sound_size) << "],";
  
  unsigned char sound_type = fmt & 0x01;
  std::cout << "sound_type[" << to_hex(sound_type) << "]" << std::endl;

  unsigned char aac_packet_type = fmt_aac_type & 0x00ff;
  std::cout << "aac_packet_type[" << to_hex(aac_packet_type) << "]" << std::endl;

  if (sound_format == FLV_AAC && 
    aac_packet_type == FLV_AAC_SEQUENCE_HEADER) {
    return true;
  }
  return false;
}

bool get_aac_header(unsigned char*& buffer, uint32_t& sample_rate, uint32_t& channel_count, std::size_t& buffer_len) {

  uint32_t SamplingRates[] = {
    96000,  // 0
    88200,
    64000,
    48000,
    44100,
    32000,
    24000,
    22050,
    16000,
    12000,
    11025,
    8000,
    7350,   //12
    0,      // 13
    0,      // 14
    0       // 15
  };

  // flv frame sound format and more : 1byte
  // AAC type : 1byte
  uint16_t fmt_aac_type;
  read_2byte(buffer, fmt_aac_type, buffer_len); 
  unsigned char fmt = (fmt_aac_type & 0xff00) >> 8;
  unsigned char sound_format = (fmt & 0xf0) >> 4;
  unsigned char aac_packet_type = fmt_aac_type & 0x00ff;

  std::cout << "fmt_aac_type[" << to_hex(fmt_aac_type) << "]->";
  std::cout << "fmt[" << to_hex(fmt) << "],";
  std::cout << "sound_format[" << to_hex(sound_format) << "]" << std::endl;;
  std::cout << "aac_packet_type[" << to_hex(aac_packet_type) << "]" << std::endl;

  if (sound_format != FLV_AAC && 
    aac_packet_type != FLV_AAC_SEQUENCE_HEADER) {
    return false;
  }

  // mp4 aac audio specific config
  // 5 bits: object type
  // if (object type == 31)
  //     6 bits + 32: object type
  // 4 bits: frequency(=sample rate) index
  // if (frequency index == 15)
  //     24 bits: frequency
  // 4 bits: channel configuration
  // var bits: AOT Specific Config
  uint16_t aac_header;
  read_2byte(buffer, aac_header, buffer_len); 
  
  uint8_t hb = (aac_header & 0xff00) >> 8;
  uint8_t lb = (aac_header & 0x00ff);

  uint8_t audio_object_type = hb >> 3 ; // high 5bit
  uint8_t sample_rate_index = 0; 
  
  sample_rate = 0; 
  uint8_t channel_config = 0;

  if (audio_object_type == 31) {
    audio_object_type = (((hb & 0x07) << 3) | ((lb) >> 5)) + 32 ; //  next 6 th bit + 32
    std::cout << "31 case: audio_object_type[" << to_hex(audio_object_type) << "]" << std::endl;

    sample_rate_index =((lb >> 1) & 0x0f);
    std::cout << "sample_rate_index[" << to_hex(sample_rate_index) << "]" << std::endl;;

    if (sample_rate_index == 0x0f) {
      uint32_t sample_rate_channel_config = 0; 
      read_3byte(buffer, sample_rate_channel_config, buffer_len);
      
      sample_rate = (lb & 0x01) << 23 | // 23 but
            ((sample_rate_channel_config) >> 1 ); // 1 bit
      
      uint8_t b3;
      read_1byte(buffer, b3, buffer_len); 
      channel_config = ((sample_rate_channel_config & 0x01) << 3) | ((b3) >> 5 );  
      
      std::cout << "sample_rate[" << sample_rate << "]" << std::endl;;
      std::cout << "channel_config[" << to_hex(channel_config) << "]" << std::endl;;

    } else {
      sample_rate = SamplingRates[sample_rate_index];
      
      uint8_t b3;
      read_1byte(buffer, b3, buffer_len); 
      channel_config = ((lb & 0x01) << 3) | ((b3) >> 5 );  

      std::cout << "sample_rate[" << sample_rate << "]" << std::endl;;
      std::cout << "channel_config[" << to_hex(channel_config) << "]" << std::endl;;
    }

  } else {

    std::cout << "audio_object_type[" << to_hex(audio_object_type) << "]" << std::endl;;

    sample_rate_index = 
          ((hb << 1) & 0x0f) | // low 3bit << 1 +
          ((lb & 0x80) >> 7);   // high 1 bit of low byte

    std::cout << "sample_rate_index[" << to_hex(sample_rate_index) << "]" << std::endl;;

    if ( sample_rate_index == 0x0f ) {
      uint32_t sample_rate_channel_config = 0; 
      read_3byte(buffer, sample_rate_channel_config, buffer_len);
      
      sample_rate = ( lb & 0x7f ) << (24-7) | // 7 bit
        ((sample_rate_channel_config & 0xffff80) >> 7 ); // 17 bit
      channel_config = ( sample_rate_channel_config >> 3 ) & 0x00000f ;

      std::cout << "sample_rate[" << sample_rate << "]" << std::endl;;
      std::cout << "channel_config[" << to_hex(channel_config) << "]" << std::endl;;

    } else {
      sample_rate = SamplingRates[sample_rate_index];
      channel_config = ( lb >> 3 ) & 0x0f;  

      std::cout << "sample_rate[" << sample_rate << "]" << std::endl;;
      std::cout << "channel_config[" << to_hex(channel_config) << "]" << std::endl;;
    }
  }

  channel_count = channel_config;
  std::cout << "channel_count[" << channel_count << "]" << std::endl;;

  return true;  
}

bool get_audio(unsigned char*& buffer, buffer_t& audio_es, audio_frame_t& audio_frame, std::size_t& buffer_len) {
  // flv frame sound format and more : 1byte
  // AAC type : 1byte
  uint16_t fmt_aac_type;
  read_2byte(buffer, fmt_aac_type, buffer_len); 
  //std::cout << "fmt_aac_type[" << to_hex(fmt_aac_type) << "]->";
  unsigned char fmt = (fmt_aac_type & 0xff00) >> 8;
  unsigned char sound_format = (fmt & 0xf0) >> 4;
  // unsigned char sound_rate = fmt & 0x0c >> 2;
  // unsigned char sound_size = fmt & 0x02 >> 1;
  // unsigned char sound_type = fmt & 0x01;


  unsigned char aac_packet_type = fmt_aac_type & 0x00ff;

  std::cout << "fmt_aac_type[" << to_hex(fmt_aac_type) << "]->";
  std::cout << "fmt[" << to_hex(fmt) << "],";
  std::cout << "sound_format[" << to_hex(sound_format) << "]" << std::endl;;
  std::cout << "aac_packet_type[" << to_hex(aac_packet_type) << "]" << std::endl;


  if (sound_format != FLV_AAC && 
    aac_packet_type != FLV_AAC_RAW) {
    return false;
  }

  read_nbyte(buffer, buffer_len, audio_es, buffer_len);
 
  return true;
}  

bool is_avc_video(unsigned char* const & buffer) {
  uint16_t fmt_avc_type;
  get_2byte(buffer, fmt_avc_type); 
  //std::cout << "fmt_avc_type[" << to_hex(fmt_avc_type) << "]->";
  
  unsigned char fmt;
  fmt = (fmt_avc_type & 0xff00) >> 8;
  unsigned char codec_id = fmt & 0x0f;
  unsigned char avc_type = fmt_avc_type & 0x00ff;

  //std::cout << "fmt[" << to_hex(fmt) << "]->";
  //unsigned char frame_type = (fmt & 0xf0) >> 4;
  //std::cout << "frame_type[" << to_hex(frame_type) << "],";
  //std::cout << "codec_id[" << to_hex(codec_id) << "]" << std::endl;
  //std::cout << "avc_type[" << to_hex(avc_type) << "]" << std::endl;


  if (codec_id == FLV_AVC && avc_type == FLV_AVC_NALU) {
    return true;
  }
  return false;
}

bool is_avc_sequence_header(unsigned char* const & buffer) {
  uint16_t fmt_avc_type;
  get_2byte(buffer, fmt_avc_type); 

  unsigned char fmt;
  fmt = (fmt_avc_type & 0xff00) >> 8;

  unsigned char codec_id = fmt & 0x0f;
  unsigned char avc_type = fmt_avc_type & 0x00ff;

  if (codec_id == FLV_AVC && avc_type == FLV_AVC_SEQUENCE_HEADER) {
    return true;
  }
  return false;
}

bool is_avc_end_of_sequence(unsigned char* const & buffer) {
  uint16_t fmt_avc_type;
  get_2byte(buffer, fmt_avc_type); 

  unsigned char fmt;
  fmt = (fmt_avc_type & 0xff00) >> 8;

  unsigned char codec_id = fmt & 0x0f;
  unsigned char avc_type = fmt_avc_type & 0x00ff;

  if (codec_id == FLV_AVC && avc_type == FLV_AVC_EOS) {
    return true;
  }
  return false;
}

std::string encoding_base_64(buffer_t &src) {
  char const *p =  reinterpret_cast<const char *>(src.ptr_);
  std::string in(p,  src.len_);
  std::string out;
  Base64::Encode(in, &out);
  
  std::cout << "src >" << std::endl;
  std::cout << to_hex(src) << std::endl;

  std::cout << "base64 encoded >" << std::endl;
  std::cout << out << std::endl;

  return out;
}

bool get_first_sps_and_first_pps_from_avc_sequence_header(unsigned char*& buffer, buffer_t& sps, buffer_t& pps, uint8_t& nal_startcode_len, std::size_t& buffer_len) {

  // flv frame type/codec id
  // H264 AVC type 
  // 0 composition time offset 1st byte
  // 0 composition time offset 2nd byte
  // 0 composition time offset 3rd byte
  // version
  // profile
  // compatibility
  // level
  // nal start cpde bytes length
  // number of sps nal 
  // for number of pps nal {
  //  sps length
  //  sps
  // }
  // number of pps nal 
  // for number of pps nal {
  //  pps length
  //  pps
  // }
  //

  unsigned char fmt;
  read_1byte(buffer, fmt, buffer_len); 
  std::cout << "fmt[" << to_hex(fmt) << "]->";
  
  unsigned char frame_type = (fmt & 0xf0) >> 4;
  std::cout << "frame_type[" << to_hex(frame_type) << "],";

  unsigned char codec_id = fmt & 0x0f;
  std::cout << "codec_id[" << to_hex(codec_id) << "]" << std::endl;
  
  unsigned char avc_type;
  read_1byte(buffer, avc_type, buffer_len); 
  std::cout << "avc_type[" << to_hex(avc_type) << "]" << std::endl;

  // check avc sequence header
  if ( avc_type != 0 )
    return false;
  
  // composition time offset
  buffer_t composition_timestamp_offset(3);
  read_nbyte(buffer, 3, composition_timestamp_offset.ptr_, buffer_len); 
  composition_timestamp_offset.len_ = 3;
  std::cout << "composition_timestamp_offset[" << to_hex(composition_timestamp_offset) << "]->";

  uint32_t cts = 
    (composition_timestamp_offset.ptr_[0] << 16) |
    (composition_timestamp_offset.ptr_[1] << 8) |
    composition_timestamp_offset.ptr_[2];

  std::cout << "cts[" << to_hex(cts) << "]->";

  cts = ((cts & 0x00FF0000) >> 16) |
      ((cts & 0x000000FF) << 16) |
      (cts & 0x0000FF00);
  std::cout << "modified cts[" << to_hex(cts) << "]" << std::endl;

  // version
  // profile
  // compatibility
  // level
  buffer_t vpcl(4);
  read_nbyte(buffer, 4, vpcl.ptr_, buffer_len);
  vpcl.len_ += 4;
  std::cout << "version, profile, compatibilty, level[" << to_hex(vpcl.ptr_, vpcl.len_) << "]" << std::endl;

  unsigned char nal_start_code_length;
  read_1byte(buffer, nal_start_code_length, buffer_len); 
  std::cout << "nal_start_code_length[" << to_hex(nal_start_code_length) << "]->";
  nal_start_code_length = ( nal_start_code_length & 0x03 ) + 1; 
  std::cout << "nal_bytes_start_code_length[" << to_hex(nal_start_code_length) << "]" << std::endl;

  nal_startcode_len = nal_start_code_length;

  unsigned char number_of_sps;
  read_1byte(buffer, number_of_sps, buffer_len); 
  std::cout << "number_of_sps[" << to_hex(number_of_sps) << "]->";
  number_of_sps &= 0x1f;
  std::cout << "number_of_sps[" << to_hex(number_of_sps) << "]" << std::endl;

  uint16_t sps_length;
  for (;number_of_sps > 0; --number_of_sps) {
    // sps length
    read_2byte(buffer, sps_length, buffer_len);
    std::cout << "sps_length[" << to_hex(sps_length) << "]" << std::endl;

    if (sps_length > sps.max_len_ ) return false;
    // sps 
    read_nbyte(buffer, sps_length, sps, buffer_len);
    std::cout << "sps[" << to_hex(sps) << "]" << std::endl;

    // get the first one only
    break;
  }

  unsigned char number_of_pps;
  read_1byte(buffer, number_of_pps, buffer_len); 
  std::cout << "number_of_pps[" << to_hex(number_of_pps) << "]" << std::endl;

  uint16_t pps_length;
  for (; number_of_pps > 0; --number_of_pps) {
    // pps length
    read_2byte(buffer, pps_length, buffer_len);
    std::cout << "pps_length[" << to_hex(pps_length) << "]" << std::endl;

    if (pps_length > pps.max_len_ ) return false;

    // pps 
    read_nbyte(buffer, pps_length, pps, buffer_len);
    std::cout << "pps[" << to_hex(pps) << "]" << std::endl;

    // get the first one only
    break;

    }

  return true;
}

void get_avc_end_of_sequence(unsigned char*& buffer, buffer_t& eos, std::size_t& buffer_len) {
  read_nbyte(buffer, buffer_len, eos, buffer_len);
}

bool get_video(unsigned char*& buffer, buffer_t& video_es, video_frame_t& video_frame, std::size_t& buffer_len) {
  // flv frame type/codec id
  // H264 AVC type 
  // 0 composition time offset 1st byte
  // 0 composition time offset 2nd byte
  // 0 composition time offset 3rd byte
  unsigned char fmt;
  read_1byte(buffer, fmt, buffer_len); 
  std::cout << "fmt[" << to_hex(fmt) << "]->";
  
  unsigned char frame_type = (fmt & 0xf0) >> 4;
  std::cout << "frame_type[" << to_hex(frame_type) << "],";

  unsigned char codec_id = fmt & 0x0f;
  std::cout << "codec_id[" << to_hex(codec_id) << "]" << std::endl;
  
  unsigned char avc_type;
  read_1byte(buffer, avc_type, buffer_len); 
  std::cout << "avc_type[" << to_hex(avc_type) << "]" << std::endl;

  // check avc nal unit
  if ( avc_type != 1 ) { return false; }
  
  // composition time offset
  uint32_t cts;
  read_3byte(buffer, cts, buffer_len);
  std::cout << "cts[" << to_hex(cts) << "]->";

  cts = ((cts & 0x00FF0000) >> 16) |
      ((cts & 0x000000FF) << 16) |
      (cts & 0x0000FF00);
  std::cout << "modified cts[" << to_hex(cts) << "]" << std::endl;

  // read nal start bytes and payload
  read_nbyte(buffer, buffer_len, video_es, buffer_len);
  
  video_frame.cts_ = cts;
  video_frame.keyframe_ = frame_type;
  video_frame.codec_id_ = codec_id;

  return true;
}  
// FIXME: 
// buffer 가 두 개 필요없는 것 같음
//
void replace_nalu_strat_code_from_mp4_to_ts(unsigned char nal_start_code_length,
  buffer_t const& mp4_video_es, buffer_t& ts_video_es)
{
  unsigned char* mp4_es_ptr = mp4_video_es.ptr_;
  std::size_t mp4_es_len = mp4_video_es.len_;

  unsigned char* ts_es_ptr = ts_video_es.ptr_;
  std::size_t ts_es_written = 0 ;

  buffer_t nal_start_code(nal_start_code_length);
  
  while ( mp4_es_len > 0 ) {
    // read start code
    read_nbyte(mp4_es_ptr, nal_start_code_length, nal_start_code, mp4_es_len); 

    // get nal length  from the last one byte of start code of 4 byte
    uint32_t nal_length = 0;
    read_nbyte(nal_start_code, nal_length);  
    
    //std::cout << "nal length >" << std::endl;
    //std::cout <<  nal_length << std::endl;

    // std::cout << "mp4 nal start code >" << std::endl;
    // std::cout << to_hex(nal_start_code) << std::endl;

    // replace last nal byte
    put_same_nbyte(nal_start_code, 0x00, nal_start_code_length - 1);
    put_last_byte(nal_start_code, 0x01);

    // write nal start code 
    write_nbyte(nal_start_code, ts_es_ptr, ts_es_written); 
    ts_video_es.len_ = ts_es_written;

    // std::cout << "ts video nal start code >" << std::endl;
    // std::cout << to_hex(ts_video_es) << std::endl;

    // write nal
    write_nbyte(mp4_es_ptr, nal_length, ts_es_ptr, mp4_es_len, ts_es_written); 
    ts_video_es.len_ = ts_es_written;

    // std::cout << "ts video nal start code >" << std::endl;
    // std::cout << to_hex(ts_video_es) << std::endl;
   
  }

}

bool read_flv_file(std::string const& file_path) {

  std::cout << "open >" << file_path << std::endl;

  std::ifstream binfile(file_path, std::ios::binary);
  if (not binfile) {
    std::cout << "> open false" << std::endl;
    return false;
  }

  // get length of file:
  binfile.seekg(0, binfile.end);
  std::size_t length = binfile.tellg();
  binfile.seekg(0, binfile.beg);
  std::cout << "> file length[" << length << "]" << std::endl;
  const std::size_t buffer_size = length;
  unsigned char* buffer = new unsigned char[buffer_size];

  std::size_t reading_size = length < buffer_size ? length : buffer_size;
  binfile.read(reinterpret_cast<char*>(buffer), reading_size);

  if (not binfile) {
    std::cout << "> read false" << std::endl;
    return false;
  }
  //std::cout << "> read length[" << reading_size << "]" << std::endl;
  std::cout << to_hex(buffer, reading_size) << std::endl;

  unsigned char* avc_seq_header_buffer = buffer;
  std::size_t avc_seq_header_buffer_len= reading_size;
  buffer_t sps(512);
  buffer_t pps(512);

  unsigned char* video_buffer = buffer;
  std::size_t video_buffer_len= reading_size;
  buffer_t video_es(video_buffer_len);
  video_frame_t video_frame;
  uint8_t nal_code_len = 4;

  bool ret = get_first_sps_and_first_pps_from_avc_sequence_header(
    avc_seq_header_buffer, sps, pps, nal_code_len, avc_seq_header_buffer_len);

  if (ret) {
    std::cout << "first sps >" << std::endl;
    std::cout << to_hex(sps) << std::endl;
    std::cout << "first pps > " << std::endl;
    std::cout << to_hex(pps) << std::endl;
  } else {
    std::cout << "> not find sps, pps" << std::endl;
    ret = get_video(video_buffer, video_es, video_frame, video_buffer_len);
    if (ret) {
      std::cout << "video_frame >" << std::endl;
      std::cout << to_string(video_frame) << std::endl;

      std::cout << "video >" << std::endl;
      std::cout << to_hex(video_es) << std::endl;
    } else {
      std::cout << "> not video data" << std::endl;
    }
  }
  
  binfile.close();
  delete[] buffer;

  return ret;
}

bool parsing_flv(std::string const& file_path) {
  
  bool ret = read_flv_file(file_path);

  return ret;
}

} // namespaace parsingh264h264