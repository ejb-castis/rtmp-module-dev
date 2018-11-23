#ifndef CONNECTMESSAGE_HPP_
#define CONNECTMESSAGE_HPP_

#include "CommandMessage.hpp"
#include <sstream>

namespace rtmp_protocol {
#define CONNECT_MSG_FIELD_STR_APP "app"
#define CONNECT_MSG_FIELD_STR_FLASHVER "flashVer"
#define CONNECT_MSG_FIELD_STR_SWFURL "swfUrl"
#define CONNECT_MSG_FIELD_STR_TCURL "tcUrl"
#define CONNECT_MSG_FIELD_STR_FPAD "fpad"
#define CONNECT_MSG_FIELD_STR_CAPABILITIES "capabilities"
#define CONNECT_MSG_FIELD_STR_AUDIOCODECS "audioCodecs"
#define CONNECT_MSG_FIELD_STR_VIDEOCODECS "videoCodecs"
#define CONNECT_MSG_FIELD_STR_VIDEOFUNCTION "videoFunction"
#define CONNECT_MSG_FIELD_STR_PAGEURL "pageUrl"
#define CONNECT_MSG_FIELD_STR_OBJECTENCODING "objectEncoding"

class ConnectMsgFieldType {
 public:
  typedef enum {
    APP,
    FLASHVER,
    SWFURL,
    TCURL,
    FPAD,
    CAPABILITIES,
    AUDIOCODECS,
    VIDEOCODECS,
    VIDEOFUNCTION,
    PAGEURL,
    OBJECTENCODING,
    UNKNOWN
  } type;

  static type get_type(std::string& str) {
    if (str.compare(CONNECT_MSG_FIELD_STR_APP) == 0)
      return APP;
    if (str.compare(CONNECT_MSG_FIELD_STR_FLASHVER) == 0)
      return FLASHVER;
    if (str.compare(CONNECT_MSG_FIELD_STR_SWFURL) == 0)
      return SWFURL;
    if (str.compare(CONNECT_MSG_FIELD_STR_TCURL) == 0)
      return TCURL;
    if (str.compare(CONNECT_MSG_FIELD_STR_FPAD) == 0)
      return FPAD;
    if (str.compare(CONNECT_MSG_FIELD_STR_CAPABILITIES) == 0)
      return CAPABILITIES;
    if (str.compare(CONNECT_MSG_FIELD_STR_AUDIOCODECS) == 0)
      return AUDIOCODECS;
    if (str.compare(CONNECT_MSG_FIELD_STR_VIDEOCODECS) == 0)
      return VIDEOCODECS;
    if (str.compare(CONNECT_MSG_FIELD_STR_VIDEOFUNCTION) == 0)
      return VIDEOFUNCTION;
    if (str.compare(CONNECT_MSG_FIELD_STR_PAGEURL) == 0)
      return PAGEURL;
    if (str.compare(CONNECT_MSG_FIELD_STR_OBJECTENCODING) == 0)
      return OBJECTENCODING;
    return UNKNOWN;
  }
  static std::string get_type_str(type value) {
    switch (value) {
      case APP:
        return CONNECT_MSG_FIELD_STR_APP;
      case FLASHVER:
        return CONNECT_MSG_FIELD_STR_FLASHVER;
      case SWFURL:
        return CONNECT_MSG_FIELD_STR_SWFURL;
      case TCURL:
        return CONNECT_MSG_FIELD_STR_TCURL;
      case FPAD:
        return CONNECT_MSG_FIELD_STR_FPAD;
      case CAPABILITIES:
        return CONNECT_MSG_FIELD_STR_CAPABILITIES;
      case AUDIOCODECS:
        return CONNECT_MSG_FIELD_STR_AUDIOCODECS;
      case VIDEOCODECS:
        return CONNECT_MSG_FIELD_STR_VIDEOCODECS;
      case VIDEOFUNCTION:
        return CONNECT_MSG_FIELD_STR_VIDEOFUNCTION;
      case PAGEURL:
        return CONNECT_MSG_FIELD_STR_PAGEURL;
      case OBJECTENCODING:
        return CONNECT_MSG_FIELD_STR_OBJECTENCODING;
      case UNKNOWN:
        return "Unknown";
    }
    return "Unknown";
  }
};

class ConnectMessage : public CommandMessage {
 public:
  ConnectMessage() {
    set_type(RtmpMessageType::CONNECT);
  }
  virtual ~ConnectMessage() {
  }
  virtual std::string get_class_name() {
    return "ConnectMessage";
  }
  typedef enum audio_codec_mask {
    SUPPORT_SND_NONE = 0x0001  //Raw sound, no compression
    ,
    SUPPORT_SND_ADPCM = 0x0002  // ADPCM compression
    ,
    SUPPORT_SND_MP3 = 0x0004  // mp3 compression
    ,
    SUPPORT_SND_INTEL = 0x0008  // Not used
    ,
    SUPPORT_SND_UNUSED = 0x0010  // Not used
    ,
    SUPPORT_SND_NELLY8 = 0x0020  // NellyMoser at 8-kHz
    ,
    SUPPORT_SND_NELLY = 0x0040  // NellyMoser compression
    ,
    SUPPORT_SND_G711A = 0x0080  // G711A sound compression
    ,
    SUPPORT_SND_G711U = 0x0100  // G711U sound compression
    ,
    SUPPORT_SND_NELLY16 = 0x0200  // NellyMouser at 16-kHz
    ,
    SUPPORT_SND_AAC = 0x0400  //| Advanced audio coding
    ,
    SUPPORT_SND_SPEEX = 0x0800  // Speex Audio
    ,
    SUPPORT_SND_ALL = 0x0FFF  // All RTMP-supported audio
  } audio_codec_mask;

  typedef enum video_codec_mask {
    SUPPORT_VID_UNUSED = 0x0001  // Obsolete value
    ,
    SUPPORT_VID_JPEG = 0x0002  // Obsolete value
    ,
    SUPPORT_VID_SORENSON = 0x0004  // Sorenson Flash video
    ,
    SUPPORT_VID_HOMEBREW = 0x0008  // V1 screen sharing
    ,
    SUPPORT_VID_VP6 = 0x0010  //On2 video (Flash 8+)
    ,
    SUPPORT_VID_VP6ALPHA = 0x0020  //On2 video with alpha
    ,
    SUPPORT_VID_HOMEBREWV = 0x0040  // Screen sharing version 2
    ,
    SUPPORT_VID_H264 = 0x0080  //| H264 video
    ,
    SUPPORT_VID_ALL = 0x00FF  // All RTMP-supported video
  } video_codec_mask;

  std::string app_;
  std::string flashver_;
  std::string swf_url_;
  std::string tc_url_;
  bool fpad_;
  int capabilities_;
  int audio_codecs_;
  int video_codecs_;
  int video_function_;
  std::string page_url_;
  int object_encoding_;

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "ConnectMessage:";
    oss << CommandMessage::to_string();
    oss << ",app:" << app_;
    oss << ", flashver:" << flashver_;
    oss << ", swf_url:" << swf_url_;
    oss << ", tc_url:" << tc_url_;
    oss << ", fpad:" << fpad_;
    oss << ", capabilities:" << capabilities_;
    oss << ", audio_codecs:" << audio_codecs_;
    oss << ", video_codecs:" << video_codecs_;
    oss << ", video_function:" << video_function_;
    oss << ", page_url:" << page_url_;
    oss << ", object_encoding:" << object_encoding_;
    return oss.str();
  }

  size_t write_body_payload(std::ostream& stream) {
    return 0;
  }
};
typedef boost::shared_ptr<ConnectMessage> ConnectMessage_ptr;
}  // namespace rtmp_protocol

#endif /* CONNECTMESSAGE_HPP_ */
