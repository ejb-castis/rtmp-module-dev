#include <algorithm>
#include <bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>

#include "flv_util.h"

#include "parsingflv.h"
#include "rtmpmodulelogger.h"
#include "streamer/base64.h"

using namespace flv_util;
using namespace flv;

#define CHECK(x)            \
  do {                      \
    if (!(x)) return false; \
  } while (0)

namespace flv {

std::string AACAudioData::ToString() const {
  static std::vector<std::string> kSoundFormat{
      "", "", "MP3", "", "", "", "",         "",
      "", "", "AAC", "", "", "", "MP3-8khz", ""};
  static std::vector<std::string> kSoundRate{"5.5khz", "11khz", "22khz",
                                             "44khz"};
  static std::vector<std::string> kSoundSize{"8bit", "16bit"};
  static std::vector<std::string> kSoundType{"Mono", "Stereo"};
  static std::vector<std::string> kAACPacketType{"AACSequenceHeader", "AACRaw"};

  auto sf =
      sound_format_ > kSoundFormat.size() ? "" : kSoundFormat[sound_format_];
  auto sr = sound_rate_ > kSoundRate.size() ? "" : kSoundRate[sound_rate_];
  auto ss = sound_size_ > kSoundSize.size() ? "" : kSoundSize[sound_size_];
  auto st = sound_type_ > kSoundType.size() ? "" : kSoundType[sound_type_];
  auto ap = aac_packet_type_ > kAACPacketType.size()
                ? ""
                : kAACPacketType[aac_packet_type_];

  auto fmt = boost::format(
                 "sound_format[%1%:%2%],sountd_rate[%3%:%4%],sound_size[%5%:%6%"
                 "],sound_type[%7%:%8%],aac_packet_type[%9%:%10%]") %
             sf % static_cast<uint16_t>(sound_format_) % sr %
             static_cast<uint16_t>(sound_rate_) % ss %
             static_cast<uint16_t>(sound_size_) % st %
             static_cast<uint16_t>(sound_type_) % ap %
             static_cast<uint16_t>(aac_packet_type_);
  return fmt.str();
}

std::string AudioSpecificConfig::ToString() const {
  static std::vector<std::string> kAudioObjectType{"Null",
                                                   "AACMain",
                                                   "AACLC",
                                                   "AACSSR",
                                                   "AACLTP",
                                                   "SBR",
                                                   "AACScalable",
                                                   "TwinVQ",
                                                   "CELP",
                                                   "HXVC",
                                                   "",
                                                   "",
                                                   "TTSI",
                                                   "MainSynthesis",
                                                   "WavetableSynthesis",
                                                   "GeneralMIDI",
                                                   "AlgorithmicSynthesis",
                                                   "ERAAC",
                                                   "",
                                                   "ERAACTLP",
                                                   "ERAACScalable",
                                                   "ERTwinVQ",
                                                   "ERBSAC",
                                                   "ERAACLD",
                                                   "ERCELP",
                                                   "ERHVXC",
                                                   "ERHILN",
                                                   "ERParametric",
                                                   "SSC",
                                                   "PS",
                                                   "MPEGSurround",
                                                   ""};

  auto aot = (auio_object_type_ > kAudioObjectType.size())
                 ? ""
                 : kAudioObjectType[auio_object_type_];
  auto sf = (sampling_frequency_index_ <= 12 || sampling_frequency_index_ == 15)
                ? std::to_string(sampling_frequency_) + "Hz"
                : "";
  auto ch = (1 <= channel_configuration_ && channel_configuration_ < 8)
                ? std::to_string(channel_count_) + "channel(s)"
                : "";

  auto fmt =
      boost::format(
          "audio_object_type[%1%:%2%],sampling_frequency[%3%:%4%],channel_"
          "count[%5%:%6%]") %
      aot % auio_object_type_ % sf %
      static_cast<uint16_t>(sampling_frequency_index_) % ch %
      static_cast<uint16_t>(channel_configuration_);
  return fmt.str();
}

std::string AVCVideoData::ToString() const {
  static std::vector<std::string> kFrameType{"keyFrame", "interFrame", "", "",
                                             ""};
  static std::vector<std::string> kCodecId{
      "JPEG", "H.263", "SV", "On2VP6", "On2VP6Alpha", "SV2", "AVC"};
  static std::vector<std::string> kAVCPacketType{"AVCSequenceHeader", "AVCNALU",
                                                 "AVCEOS"};

  auto ft = frame_type_ < kFrameType.size() ? kFrameType[frame_type_] :  "" ;
  auto ci = codec_id_ < kCodecId.size() ? kCodecId[codec_id_] : "" ;
  auto ap = avc_packet_type_ < kAVCPacketType.size()
                ? 
                kAVCPacketType[avc_packet_type_] : "" ;
  return ft +","+ ci +","+ ap;
  // auto fmt = boost::format(
  //                "frame_type[%1%:%2%],codec_id[%3%:%4%],aac_packet_type[%5%:%6%"
  //                "],composition_time_offset[%7%]") %
  //            ft % static_cast<uint16_t>(frame_type_) % ci %
  //            static_cast<uint16_t>(codec_id_) % ap %
  //            static_cast<uint16_t>(avc_packet_type_) % composition_time_offset_;
  // return fmt.str();
}
std::string AVCDecoderConfigurationRecord::ToString() {
  return "";
  auto fmt = boost::format(
                 "version[%1%],profile_indication[%2%],profile_compatibility[%"
                 "3%],avc_level[%4%],nal_start_code_length[%5%],num_of_sps[%6%]"
                 ",num_of_pps_[%7%],sps_list[%8%],pps_list[%9%]") %
             static_cast<uint16_t>(version_) %
             static_cast<uint16_t>(profile_indication_) %
             static_cast<uint16_t>(profile_compatibility_) %
             static_cast<uint16_t>(avc_level_) %
             static_cast<uint16_t>(nal_start_code_length_) %
             static_cast<uint16_t>(num_of_sps_) %
             static_cast<uint16_t>(num_of_pps_) % to_hex(sps_list_) %
             to_hex(pps_list_);
  return fmt.str();
}

}  // namespace flv

namespace parsingflv {

enum FLV_FRAMETYPE { FLV_KEY_FRAME = 1, FLV_INNER_FRAME = 2 };
enum FLV_CODECID { FLV_AVC = 7 };
enum FLV_AVC_PACKETTYPE {
  FLV_AVC_SEQUENCE_HEADER = 0,
  FLV_AVC_NALU = 1,
  FLV_AVC_EOS = 2
};
enum FLV_SOUND_FORMAT { FLV_AAC = 10 };
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
  oss << "cts[" << frame.cts_ << "],";
  oss << "keyframe[" << frame.keyframe_ << "],";
  oss << "codec_id[" << frame.codec_id_ << "]";
  return oss.str();
}
std::string to_string(audio_frame_t& frame) {
  std::ostringstream oss;
  oss << "timestamp[" << frame.timestamp_ << "],";
  oss << "sample_rate[" << frame.sample_rate_ << "],";
  oss << "channel_count[" << frame.channel_count_ << "],";
  oss << "codec_id[" << frame.codec_id_ << "]";
  return oss.str();
}

bool is_aac_audio(unsigned char* const& buffer) {
  uint16_t fmt_aac_type;
  get_2byte(buffer, fmt_aac_type);
  unsigned char fmt = (fmt_aac_type & 0xff00) >> 8;
  unsigned char sound_format = (fmt & 0xf0) >> 4;
  unsigned char aac_packet_type = fmt_aac_type & 0x00ff;
  if (sound_format == FLV_AAC && aac_packet_type == FLV_AAC_RAW) {
    return true;
  }
  return false;
}

bool is_aac_sequence_header(unsigned char* const& buffer) {
  uint16_t fmt_aac_type;
  get_2byte(buffer, fmt_aac_type);
  unsigned char fmt = (fmt_aac_type & 0xff00) >> 8;
  unsigned char sound_format = (fmt & 0xf0) >> 4;
  unsigned char sound_rate = fmt & 0x0c >> 2;
  unsigned char sound_size = fmt & 0x02 >> 1;
  unsigned char sound_type = fmt & 0x01;
  unsigned char aac_packet_type = fmt_aac_type & 0x00ff;
  if (sound_format == FLV_AAC && aac_packet_type == FLV_AAC_SEQUENCE_HEADER) {
    return true;
  }
  return false;
}

bool get_aac_header(unsigned char*& buffer, uint32_t& obejct_type,
                    uint32_t& sample_rate, uint32_t& channel_count,
                    std::size_t& buffer_len) {
  uint32_t SamplingRates[] = {
      96000,  // 0
      88200, 64000, 48000, 44100, 32000, 24000,
      22050, 16000, 12000, 11025, 8000,
      7350,  // 12
      0,     // 13
      0,     // 14
      0      // 15
  };

  // flv frame sound format and more : 1byte
  // AAC type : 1byte
  CHECK(buffer_len >= 2);

  uint16_t fmt_aac_type;
  read_2byte(buffer, fmt_aac_type, buffer_len);
  unsigned char fmt = (fmt_aac_type & 0xff00) >> 8;
  unsigned char sound_format = (fmt & 0xf0) >> 4;
  unsigned char aac_packet_type = fmt_aac_type & 0x00ff;

  if (sound_format != FLV_AAC || aac_packet_type != FLV_AAC_SEQUENCE_HEADER) {
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
  CHECK(buffer_len >= 2);

  uint16_t aac_header;
  read_2byte(buffer, aac_header, buffer_len);

  uint8_t hb = (aac_header & 0xff00) >> 8;
  uint8_t lb = (aac_header & 0x00ff);

  uint8_t audio_object_type = hb >> 3;  // high 5bit
  uint8_t sample_rate_index = 0;

  sample_rate = 0;
  uint8_t channel_config = 0;

  if (audio_object_type == 31) {
    audio_object_type =
        (((hb & 0x07) << 3) | ((lb) >> 5)) + 32;  //  next 6 th bit + 32

    sample_rate_index = ((lb >> 1) & 0x0f);

    if (sample_rate_index == 0x0f) {
      CHECK(buffer_len >= 3);
      uint32_t sample_rate_channel_config = 0;
      read_3byte(buffer, sample_rate_channel_config, buffer_len);

      sample_rate = (lb & 0x01) << 23 |                   // 23 but
                    ((sample_rate_channel_config) >> 1);  // 1 bit

      CHECK(buffer_len >= 1);
      uint8_t b3;
      read_1byte(buffer, b3, buffer_len);
      channel_config = ((sample_rate_channel_config & 0x01) << 3) | ((b3) >> 5);

    } else {
      sample_rate = SamplingRates[sample_rate_index];

      CHECK(buffer_len >= 1);
      uint8_t b3;
      read_1byte(buffer, b3, buffer_len);
      channel_config = ((lb & 0x01) << 3) | ((b3) >> 5);
    }

  } else {
    sample_rate_index = ((hb << 1) & 0x0f) |  // low 3bit << 1 +
                        ((lb & 0x80) >> 7);   // high 1 bit of low byte

    if (sample_rate_index == 0x0f) {
      CHECK(buffer_len > 3);
      uint32_t sample_rate_channel_config = 0;
      read_3byte(buffer, sample_rate_channel_config, buffer_len);

      sample_rate = (lb & 0x7f) << (24 - 7) |                        // 7 bit
                    ((sample_rate_channel_config & 0xffff80) >> 7);  // 17 bit
      channel_config = (sample_rate_channel_config >> 3) & 0x00000f;

    } else {
      sample_rate = SamplingRates[sample_rate_index];
      channel_config = (lb >> 3) & 0x0f;
    }
  }

  channel_count = channel_config;
  obejct_type = audio_object_type;

  return true;
}

bool get_audio(unsigned char*& buffer, buffer_t& audio_es,
               audio_frame_t& audio_frame, std::size_t& buffer_len) {
  uint16_t fmt_aac_type;
  read_2byte(buffer, fmt_aac_type, buffer_len);
  unsigned char fmt = (fmt_aac_type & 0xff00) >> 8;
  unsigned char sound_format = (fmt & 0xf0) >> 4;
  unsigned char aac_packet_type = fmt_aac_type & 0x00ff;
  if (sound_format != FLV_AAC || aac_packet_type != FLV_AAC_RAW) {
    return false;
  }
  read_nbyte(buffer, buffer_len, audio_es, buffer_len);
  return true;
}

bool is_avc_video(unsigned char* const& buffer) {
  uint16_t fmt_avc_type;
  get_2byte(buffer, fmt_avc_type);
  unsigned char fmt;
  fmt = (fmt_avc_type & 0xff00) >> 8;
  unsigned char codec_id = fmt & 0x0f;
  unsigned char avc_type = fmt_avc_type & 0x00ff;
  if (codec_id == FLV_AVC && avc_type == FLV_AVC_NALU) {
    return true;
  }
  return false;
}

bool is_avc_sequence_header(unsigned char* const& buffer) {
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

bool is_avc_end_of_sequence(unsigned char* const& buffer) {
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

std::string encoding_base_64(buffer_t& src) {
  char const* p = reinterpret_cast<const char*>(src.ptr_);
  std::string in(p, src.len_);
  std::string out;
  Base64::Encode(in, &out);

  // std::cout << "src >" << std::endl;
  // std::cout << to_hex(src) << std::endl;

  // std::cout << "base64 encoded >" << std::endl;
  // std::cout << out << std::endl;

  return out;
}

bool get_first_sps_and_first_pps_from_avc_sequence_header(
    unsigned char*& buffer, buffer_t& sps, buffer_t& pps,
    uint8_t& nal_startcode_len, std::size_t& buffer_len) {
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
  CHECK(buffer_len >= 1);
  read_1byte(buffer, fmt, buffer_len);
  // std::cout << "fmt[" << to_hex(fmt) << "]->";

  unsigned char frame_type = (fmt & 0xf0) >> 4;
  // std::cout << "frame_type[" << to_hex(frame_type) << "],";

  unsigned char codec_id = fmt & 0x0f;
  // std::cout << "codec_id[" << to_hex(codec_id) << "]" << std::endl;

  unsigned char avc_type;
  CHECK(buffer_len >= 1);
  read_1byte(buffer, avc_type, buffer_len);
  // std::cout << "avc_type[" << to_hex(avc_type) << "]" << std::endl;

  // check avc sequence header
  if (avc_type != 0) return false;

  // composition time offset
  uint32_t cts;
  CHECK(buffer_len >= 3);
  read_3byte(buffer, cts, buffer_len);

  // std::cout << "cts[" << to_hex(cts) << "]" << std::endl;

  // version
  // profile
  // compatibility
  // level
  buffer_t vpcl(4);
  CHECK(buffer_len >= 4);
  read_nbyte(buffer, 4, vpcl.ptr_, buffer_len);
  vpcl.len_ += 4;
  // std::cout << "version, profile, compatibilty, level[" << to_hex(vpcl.ptr_,
  // vpcl.len_) << "]" << std::endl;

  unsigned char nal_start_code_length;
  CHECK(buffer_len >= 1);
  read_1byte(buffer, nal_start_code_length, buffer_len);
  // std::cout << "nal_start_code_length[" << to_hex(nal_start_code_length) <<
  // "]->";
  nal_start_code_length = (nal_start_code_length & 0x03) + 1;
  // std::cout << "nal_bytes_start_code_length[" <<
  // to_hex(nal_start_code_length) << "]" << std::endl;

  nal_startcode_len = nal_start_code_length;

  unsigned char number_of_sps;
  CHECK(buffer_len >= 1);
  read_1byte(buffer, number_of_sps, buffer_len);
  // std::cout << "number_of_sps[" << to_hex(number_of_sps) << "]->";
  number_of_sps &= 0x1f;
  // std::cout << "number_of_sps[" << to_hex(number_of_sps) << "]" << std::endl;

  uint16_t sps_length;
  for (; number_of_sps > 0; --number_of_sps) {
    // sps length
    CHECK(buffer_len >= 2);
    read_2byte(buffer, sps_length, buffer_len);
    // std::cout << "sps_length[" << to_hex(sps_length) << "]" << std::endl;

    if (sps_length > sps.max_len_) return false;
    // sps
    CHECK(buffer_len >= sps_length);
    read_nbyte(buffer, sps_length, sps, buffer_len);
    // std::cout << "sps[" << to_hex(sps) << "]" << std::endl;

    // get the first one only
    break;
  }

  unsigned char number_of_pps;
  CHECK(buffer_len >= 1);
  read_1byte(buffer, number_of_pps, buffer_len);
  // std::cout << "number_of_pps[" << to_hex(number_of_pps) << "]" << std::endl;

  uint16_t pps_length;
  for (; number_of_pps > 0; --number_of_pps) {
    // pps length
    CHECK(buffer_len >= 2);
    read_2byte(buffer, pps_length, buffer_len);
    // std::cout << "pps_length[" << to_hex(pps_length) << "]" << std::endl;

    if (pps_length > pps.max_len_) return false;

    // pps
    CHECK(buffer_len >= pps_length);
    read_nbyte(buffer, pps_length, pps, buffer_len);
    // std::cout << "pps[" << to_hex(pps) << "]" << std::endl;

    // get the first one only
    break;
  }

  return true;
}

void get_avc_end_of_sequence(unsigned char*& buffer, buffer_t& eos,
                             std::size_t& buffer_len) {
  read_nbyte(buffer, buffer_len, eos, buffer_len);
}

bool get_video(unsigned char*& buffer, buffer_t& video_es,
               video_frame_t& video_frame, std::size_t& buffer_len) {
  // flv frame type/codec id
  // H264 AVC type
  // 0 composition time offset 1st byte
  // 0 composition time offset 2nd byte
  // 0 composition time offset 3rd byte

  // frameType :
  // 1 : keyframe (for AVC, a seekable frame)
  // 2 : interframe (for AVC, a non-seekable frame)
  // 3 : disposable inter frame (H.263 only)
  // 4 : generated key frame (reserved for server use only)
  // 5: video info/command frame

  // codecId  :
  // 1 : JPEC
  // 2 : Sorenson H.263
  // 3 : Screen Video
  // 4 : On2 VP6
  // 5: On2 VP6 with alpha channel
  // 6: Screen video version 2
  // 7: AVC

  // AVCPacketType  :  H264 AVC type
  // 0 : AVC sequence header
  // 1 : AVC NANU
  // 2 : AVC end of sequence

  unsigned char fmt;
  CHECK(buffer_len >= 1);
  read_1byte(buffer, fmt, buffer_len);
  // std::cout << "fmt[" << to_hex(fmt) << "]->";

  unsigned char frame_type = (fmt & 0xf0) >> 4;
  // std::cout << "frame_type[" << to_hex(frame_type) << "],";

  unsigned char codec_id = fmt & 0x0f;
  // std::cout << "codec_id[" << to_hex(codec_id) << "]" << std::endl;

  unsigned char avc_type;
  CHECK(buffer_len >= 1);
  read_1byte(buffer, avc_type, buffer_len);
  // std::cout << "avc_type[" << to_hex(avc_type) << "]" << std::endl;

  // check avc nal unit
  if (avc_type != 1) {
    return false;
  }

  // composition time offset : big endian
  uint32_t cts;
  CHECK(buffer_len >= 3);
  read_3byte(buffer, cts, buffer_len);
  // std::cout << "cts[" << to_hex(cts) << "]" << std::endl;

  // read nal start bytes and payload
  read_nbyte(buffer, buffer_len, video_es, buffer_len);

  video_frame.cts_ = cts;
  video_frame.keyframe_ = frame_type;
  video_frame.codec_id_ = codec_id;

  return true;
}

void replace_nalu_start_code_from_mp4_to_ts(
    unsigned char nal_start_code_length, flv_util::buffer_t const& video_es) {
  unsigned char* mp4_es_ptr = video_es.ptr_;
  std::size_t left_mp4_es_len = video_es.len_;

  unsigned char* ts_es_ptr = video_es.ptr_;
  std::size_t ts_es_written = 0;

  buffer_t ts_nal_start_code(nal_start_code_length);
  ts_nal_start_code.len_ = nal_start_code_length;
  put_same_nbyte(ts_nal_start_code, 0x00, nal_start_code_length - 1);
  put_last_byte(ts_nal_start_code, 0x01);

  // std::cout << "ts nal start code:" << to_hex(ts_nal_start_code) <<
  // std::endl;

  buffer_t nal_start_code(nal_start_code_length);

  uint32_t nal_length = 0;
  while (left_mp4_es_len > 0) {
    // std::cout << "s:" << left_mp4_es_len << std::endl;
    // read mp4 nal start code
    read_nbyte(mp4_es_ptr, nal_start_code_length, nal_start_code,
               left_mp4_es_len);
    nal_length = 0;
    read_nbyte(nal_start_code, nal_length);

    // std::cout << left_mp4_es_len << std::endl;

    // write ts nal start code
    write_nbyte(ts_nal_start_code, ts_es_ptr, ts_es_written);

    // move to next nal start code
    mp4_es_ptr += nal_length;
    left_mp4_es_len -= nal_length;
    // move to next nal start code
    ts_es_ptr += nal_length;
    ts_es_written += nal_length;

    // std::cout << "nal len:" << nal_length << std::endl;
    // std::cout << "e :" << left_mp4_es_len << std::endl;
  }

  // std::cout << to_hex(video_es) << std::endl;
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
  // std::cout << "> read length[" << reading_size << "]" << std::endl;
  std::cout << to_hex(buffer, reading_size) << std::endl;

  unsigned char* avc_seq_header_buffer = buffer;
  std::size_t avc_seq_header_buffer_len = reading_size;
  buffer_t sps(512);
  buffer_t pps(512);

  unsigned char* video_buffer = buffer;
  std::size_t video_buffer_len = reading_size;
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

bool IsAACSequenceHeader(unsigned char* const& buffer,
                         const std::size_t buffer_len) {
  uint16_t fmt_aac_type;
  CHECK(buffer_len >= 2);
  get_2byte(buffer, fmt_aac_type);
  uint8_t fmt = (fmt_aac_type & 0xff00) >> 8;
  uint8_t sound_format = (fmt & 0xf0) >> 4;
  uint8_t aac_packet_type = fmt_aac_type & 0x00ff;
  CHECK(sound_format == FLV_AAC);
  CHECK(aac_packet_type == FLV_AAC_SEQUENCE_HEADER);
  return true;
}

bool IsAACRaw(unsigned char* const& buffer, const std::size_t buffer_len) {
  uint16_t fmt_aac_type;
  CHECK(buffer_len >= 2);
  get_2byte(buffer, fmt_aac_type);
  uint8_t fmt = (fmt_aac_type & 0xff00) >> 8;
  uint8_t sound_format = (fmt & 0xf0) >> 4;
  uint8_t aac_packet_type = fmt_aac_type & 0x00ff;
  CHECK(sound_format == FLV_AAC);
  CHECK(aac_packet_type == FLV_AAC_RAW);
  return true;
}

bool ParseAACSequenceHeader(unsigned char*& buffer,
                            flv::AACAudioData& audio_data,
                            flv::AudioSpecificConfig& audio_spec_config,
                            std::size_t& buffer_len) {
  uint32_t kSamplingFrequencyTable[] = {
      96000,  // 0
      88200, 64000, 48000, 44100, 32000, 24000,
      22050, 16000, 12000, 11025, 8000,
      7350,  // 12
      0,     // 13
      0,     // 14
      0      // 15
  };
  uint32_t kChannelConfigTable[] = {0, 1, 2, 3, 4, 5, 6, 8};

  uint16_t fmt_aac_type;
  CHECK(buffer_len >= 2);
  read_2byte(buffer, fmt_aac_type, buffer_len);
  uint8_t fmt = (fmt_aac_type & 0xff00) >> 8;
  audio_data.sound_format_ = (fmt & 0xf0) >> 4;
  audio_data.sound_rate_ = fmt & 0x0c >> 2;
  audio_data.sound_size_ = fmt & 0x02 >> 1;
  audio_data.sound_type_ = fmt & 0x01;
  audio_data.aac_packet_type_ = fmt_aac_type & 0x00ff;
  CHECK(audio_data.sound_format_ == FLV_AAC);
  CHECK(audio_data.aac_packet_type_ == FLV_AAC_SEQUENCE_HEADER);

  // AudioSpecificConfig , ISO 14496-3
  CHECK(buffer_len >= 4);
  std::vector<std::uint8_t> buf(buffer, buffer + buffer_len);
  flv_util::BitReader bit_reader(buf);
  std::uint8_t audio_object_type = bit_reader.ReadBits(5);
  CHECK(1 <= audio_object_type && audio_object_type < 31);
  audio_spec_config.auio_object_type_ = audio_object_type;

  std::uint8_t sampling_frequency_index = bit_reader.ReadBits(4);
  std::uint32_t sampling_frequency{0};
  if (sampling_frequency_index == 0x0f) {
    CHECK(buffer_len >= 8);
    sampling_frequency = bit_reader.ReadBits(24);
  } else {
    sampling_frequency = kSamplingFrequencyTable[sampling_frequency_index];
  }
  audio_spec_config.sampling_frequency_index_ = sampling_frequency_index;
  audio_spec_config.sampling_frequency_ = sampling_frequency;

  std::uint8_t channel_configurations = bit_reader.ReadBits(4);
  CHECK(1 <= channel_configurations && channel_configurations < 8);
  audio_spec_config.channel_configuration_ = channel_configurations;
  audio_spec_config.channel_count_ =
      kChannelConfigTable[channel_configurations];

  return true;
}

bool ParseAACRaw(unsigned char*& buffer, flv::AACAudioData& audio_data,
                 buffer_t& audio_es, std::size_t& buffer_len) {
  uint16_t fmt_aac_type;
  CHECK(buffer_len >= 2);
  read_2byte(buffer, fmt_aac_type, buffer_len);
  uint8_t fmt = (fmt_aac_type & 0xff00) >> 8;
  audio_data.sound_format_ = (fmt & 0xf0) >> 4;
  audio_data.sound_rate_ = fmt & 0x0c >> 2;
  audio_data.sound_size_ = fmt & 0x02 >> 1;
  audio_data.sound_type_ = fmt & 0x01;
  audio_data.aac_packet_type_ = fmt_aac_type & 0x00ff;

  CHECK(audio_data.sound_format_ == FLV_AAC);
  CHECK(audio_data.aac_packet_type_ == FLV_AAC_RAW);

  read_nbyte(buffer, buffer_len, audio_es, buffer_len);

  return true;
}

bool IsAVCSequenceHeader(unsigned char* const& buffer,
                         const std::size_t buffer_len) {
  uint16_t fmt_avc_type;
  CHECK(buffer_len >= 2);
  get_2byte(buffer, fmt_avc_type);
  uint8_t fmt = (fmt_avc_type & 0xff00) >> 8;
  uint8_t codec_id = fmt & 0x0f;
  uint8_t avc_packet_type = fmt_avc_type & 0x00ff;

  CHECK(codec_id == FLV_AVC);
  CHECK(avc_packet_type == FLV_AVC_SEQUENCE_HEADER);
  return true;
}

bool IsAVCNALU(unsigned char* const& buffer, const std::size_t buffer_len) {
  uint16_t fmt_avc_type;
  CHECK(buffer_len >= 2);
  get_2byte(buffer, fmt_avc_type);
  uint8_t fmt = (fmt_avc_type & 0xff00) >> 8;
  uint8_t codec_id = fmt & 0x0f;
  uint8_t avc_packet_type = fmt_avc_type & 0x00ff;

  CHECK(codec_id == FLV_AVC);
  CHECK(avc_packet_type == FLV_AVC_NALU);
  return true;
}

bool IsAVCEndOfSequence(unsigned char* const& buffer,
                        const std::size_t buffer_len) {
  uint16_t fmt_avc_type;
  CHECK(buffer_len >= 2);
  get_2byte(buffer, fmt_avc_type);
  uint8_t fmt = (fmt_avc_type & 0xff00) >> 8;
  uint8_t codec_id = fmt & 0x0f;
  uint8_t avc_packet_type = fmt_avc_type & 0x00ff;

  CHECK(codec_id == FLV_AVC);
  CHECK(avc_packet_type == FLV_AVC_EOS);
  return true;
}

bool ParseAVCSequenceHeader(
    unsigned char*& buffer, flv::AVCVideoData& video_data,
    flv::AVCDecoderConfigurationRecord& avc_decoder_config,
    std::size_t& buffer_len) {
  std::size_t kSpsMaxLen = 0xffff;
  std::size_t kPpsMaxLen = 0xffff;
  unsigned char kDelimeter[] = {0x00, 0x00, 0x00, 0x01};

  unsigned char fmt;
  CHECK(buffer_len >= 1);
  read_1byte(buffer, fmt, buffer_len);

  video_data.frame_type_ = (fmt & 0xf0) >> 4;
  video_data.codec_id_ = fmt & 0x0f;
  CHECK(video_data.codec_id_ == FLV_AVC);

  CHECK(buffer_len >= 1);
  read_1byte(buffer, video_data.avc_packet_type_, buffer_len);
  CHECK(video_data.avc_packet_type_ == FLV_AVC_SEQUENCE_HEADER);

  CHECK(buffer_len >= 3);
  read_3byte(buffer, video_data.composition_time_offset_, buffer_len);
  CHECK(video_data.composition_time_offset_ == 0);

  CHECK(buffer_len >= 4);
  read_1byte(buffer, avc_decoder_config.version_, buffer_len);
  read_1byte(buffer, avc_decoder_config.profile_indication_, buffer_len);
  read_1byte(buffer, avc_decoder_config.profile_compatibility_, buffer_len);
  read_1byte(buffer, avc_decoder_config.avc_level_, buffer_len);

  uint8_t nal_start_code_length{4};
  CHECK(buffer_len >= 1);
  read_1byte(buffer, nal_start_code_length, buffer_len);
  CHECK((nal_start_code_length & 0x03) != 2);
  avc_decoder_config.nal_start_code_length_ =
      (nal_start_code_length & 0x03) + 1;

  CHECK(buffer_len >= 1);
  read_1byte(buffer, avc_decoder_config.num_of_sps_, buffer_len);
  avc_decoder_config.num_of_sps_ = avc_decoder_config.num_of_sps_ & 0x1f;
  CHECK(avc_decoder_config.num_of_sps_ >= 1);

  uint16_t sps_length;
  for (auto i = 0; i < avc_decoder_config.num_of_sps_; ++i) {
    CHECK(buffer_len >= 2);
    read_2byte(buffer, sps_length, buffer_len);
    CHECK(sps_length <= kSpsMaxLen);
    buffer_t sps(sps_length);
    CHECK(buffer_len >= sps_length);

    avc_decoder_config.sps_list_.insert(avc_decoder_config.sps_list_.end(),
                                        kDelimeter, kDelimeter + 4);
    read_nbyte(buffer, sps_length, sps, buffer_len);
    avc_decoder_config.sps_list_.insert(avc_decoder_config.sps_list_.end(),
                                        sps.ptr_, sps.ptr_ + sps_length);
  }

  CHECK(buffer_len >= 1);
  read_1byte(buffer, avc_decoder_config.num_of_pps_, buffer_len);
  CHECK(avc_decoder_config.num_of_sps_ >= 1);

  uint16_t pps_length;
  for (auto i = 0; i < avc_decoder_config.num_of_pps_; ++i) {
    CHECK(buffer_len >= 2);
    read_2byte(buffer, pps_length, buffer_len);
    CHECK(pps_length <= kPpsMaxLen);
    buffer_t pps(pps_length);
    CHECK(buffer_len >= pps_length);

    avc_decoder_config.pps_list_.insert(avc_decoder_config.pps_list_.end(),
                                        kDelimeter, kDelimeter + 4);
    read_nbyte(buffer, pps_length, pps, buffer_len);
    avc_decoder_config.pps_list_.insert(avc_decoder_config.pps_list_.end(),
                                        pps.ptr_, pps.ptr_ + pps_length);
  }

  return true;
}

// flv frame type
// codec id
// H264 AVC type
// 0 composition time offset 1st byte
// 0 composition time offset 2nd byte
// 0 composition time offset 3rd byte

// frameType :
// 1 : keyframe (for AVC, a seekable frame)
// 2 : interframe (for AVC, a non-seekable frame)
// 3 : disposable inter frame (H.263 only)
// 4 : generated key frame (reserved for server use only)
// 5: video info/command frame

// codecId  :
// 1 : JPEC
// 2 : Sorenson H.263
// 3 : Screen Video
// 4 : On2 VP6
// 5: On2 VP6 with alpha channel
// 6: Screen video version 2
// 7: AVC

// AVCPacketType  :  H264 AVC type
// 0 : AVC sequence header
// 1 : AVC NALU
// 2 : AVC end of sequence
bool ParseAVCNALU(unsigned char*& buffer, flv::AVCVideoData& video_data, buffer_t& video_es,
                  std::size_t& buffer_len) {
  unsigned char fmt;
  CHECK(buffer_len >= 1);
  read_1byte(buffer, fmt, buffer_len);
  
  video_data.frame_type_ = (fmt & 0xf0) >> 4;
  video_data.codec_id_ = fmt & 0x0f;
  CHECK(video_data.codec_id_ == FLV_AVC);

  CHECK(buffer_len >= 1);
  read_1byte(buffer, video_data.avc_packet_type_, buffer_len);
  CHECK(video_data.avc_packet_type_ == FLV_AVC_NALU);

  // composition time offset : big endian
  CHECK(buffer_len >= 3);
  read_3byte(buffer, video_data.composition_time_offset_, buffer_len);

  // read nal start bytes and payload
  read_nbyte(buffer, buffer_len, video_es, buffer_len);

  return true;
}

bool ParseAVCEndOfSequence(unsigned char*& buffer, flv::AVCVideoData& video_data,
                           buffer_t& eos, std::size_t& buffer_len) {
  unsigned char fmt;
  CHECK(buffer_len >= 1);
  read_1byte(buffer, fmt, buffer_len);

  video_data.frame_type_ = (fmt & 0xf0) >> 4;
  video_data.codec_id_ = fmt & 0x0f;
  CHECK(video_data.codec_id_ == FLV_AVC);

  CHECK(buffer_len >= 1);
  read_1byte(buffer, video_data.avc_packet_type_, buffer_len);
  CHECK(video_data.avc_packet_type_ == FLV_AVC_EOS);

  read_nbyte(buffer, buffer_len, eos, buffer_len);

  return true;
}

}  // namespace parsingflv