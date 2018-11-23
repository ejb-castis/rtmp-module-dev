#ifndef ONMETADATA2_HPP_
#define ONMETADATA2_HPP_

#include "DataMessage.hpp"

namespace rtmp_protocol {

#define ONMETADATA_STR_WIDTH "width"
#define ONMETADATA_STR_HEIGHT "height"
#define ONMETADATA_STR_SERVER "Server"
#define ONMETADATA_STR_BANDWIDTH "bandwidth"

#define ONMETADATA_STR_DESCRIPTION "description"
#define ONMETADATA_STR_FRAMERATE "framerate"
#define ONMETADATA_STR_VIDEOCODECID "videocodecid"
#define ONMETADATA_STR_AUDIOCODECID "audiocodecid"
#define ONMETADATA_STR_CREATIONDATE "creationdate"
#define ONMETADATA_STR_AUDIOCHANNELS "audiochannels"
#define ONMETADATA_STR_AUDIODATARATE "audiodatarate"
#define ONMETADATA_STR_AUDIODEVICE "audiodevice"
#define ONMETADATA_STR_AUDIOINPUTVOLUME "audioinputvolume"
#define ONMETADATA_STR_AUDIOSAMPLERATE "audiosamplerate"
#define ONMETADATA_STR_AUTHOR "author"
#define ONMETADATA_STR_AVCLEVEL "avclevel"
#define ONMETADATA_STR_AVCPROFILE "avcprofile"
#define ONMETADATA_STR_COPYRIGHT "copyright"
#define ONMETADATA_STR_KEYWORDS "keywords"
#define ONMETADATA_STR_PRESETNAME "presetname"
#define ONMETADATA_STR_RATING "rating"
#define ONMETADATA_STR_TITLE "title"
#define ONMETADATA_STR_VIDEODATARATE "videodatarate"
#define ONMETADATA_STR_VIDEODEVICE "videodevice"
#define ONMETADATA_STR_VIDEOKEYFRAME "videokeyframe_frequency"

class MediaMetaDataType {
 public:
  typedef enum {
    SIMPLE,
    DETAIL
  } type;

  static std::string to_string(type val) {
    switch (val) {
      case SIMPLE:
        return std::string("simple");
      case DETAIL:
        return std::string("detail");
      default:
        return std::string("unknown");
    }
  }
};

class MediaMetaData {
 private:
  MediaMetaDataType::type data_type_;

  size_t write_payload_simple(std::ostream& stream);
  size_t write_payload_detail(std::ostream& stream);

 public:
  explicit MediaMetaData(MediaMetaDataType::type data_type)
      : data_type_(data_type) {
  }
  virtual ~MediaMetaData() {
  }

  double width_;
  double height_;
  std::string server_;
  double bandwidth_;

  std::string description_;
  double framerate_;
  std::string video_codec_id_;
  std::string audio_codec_id_;
  std::string creation_date_;
  double audio_channels_;
  double audio_data_rate_;
  std::string audio_device_;
  double audio_input_volume_;
  double audio_sample_rate_;
  std::string author_;
  double avc_level_;
  double avc_profile_;
  std::string copyright_;
  std::string keywords_;
  std::string preset_name_;
  std::string rating_;
  std::string title_;
  double video_data_rate_;
  std::string video_device_;
  double video_key_frame_frequency_;

  std::string to_string() {
    std::ostringstream oss;
    oss << "media_meta_data:";
    oss << "data_type:" << MediaMetaDataType::to_string(data_type_);
    switch (data_type_) {
      case MediaMetaDataType::SIMPLE:
        oss << ",width:" << width_;
        oss << ",height:" << height_;
        oss << ",server:" << server_;
        oss << ",bandwidth:" << bandwidth_;
        break;
      case MediaMetaDataType::DETAIL:
        oss << ",description:" << description_;
        oss << ",framerate:" << framerate_;
        oss << ",video_codec_id:" << video_codec_id_;
        oss << ",audio_codec_id:" << audio_codec_id_;
        oss << ",creation_date:" << creation_date_;
        oss << ",audio_channels:" << audio_channels_;
        oss << ",audio_data_rate:" << audio_data_rate_;
        oss << ",audio_device:" << audio_device_;
        oss << ",audio_input_volume:" << audio_input_volume_;
        oss << ",audio_sample_rate:" << audio_sample_rate_;
        oss << ",author:" << author_;
        oss << ",avc_level:" << avc_level_;
        oss << ",avc_profile:" << avc_profile_;
        oss << ",copyright:" << copyright_;
        oss << ",keywords:" << keywords_;
        oss << ",preset_name:" << preset_name_;
        oss << ",rating:" << rating_;
        oss << ",title:" << title_;
        oss << ",video_data_rate:" << video_data_rate_;
        oss << ",video_device:" << video_device_;
        oss << ",video_key_frame_frequency:" << video_key_frame_frequency_;
        break;
    }
    return oss.str();
  }

  size_t write_payload(std::ostream& stream);
};

class OnMetaData : public DataMessage {
 private:
  MediaMetaData data_;

 public:
  OnMetaData()
      : data_(MediaMetaDataType::SIMPLE) {
    set_type(RtmpMessageType::ON_METADATA);
  }
  OnMetaData(RtmpHeaderFormat::format_type header_type,
             unsigned int chunk_stream_id, unsigned int msg_stream_id,
             unsigned int write_chunk_size, MediaMetaData& data)
      : data_(data) {
    set_header_format_type(header_type);
    set_type(RtmpMessageType::ON_METADATA);
    set_write_chunk_size(write_chunk_size);
    set_chunk_stream_id(chunk_stream_id);
    set_msg_stream_id(msg_stream_id);
  }
  virtual ~OnMetaData() {
  }
  virtual std::string get_class_name() {
    return "OnMetaData";
  }
  virtual size_t write_body_payload(std::ostream& stream);

  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "OnMetaData:";
    oss << DataMessage::to_string();
    oss << "," << data_.to_string();
    return oss.str();
  }
};
typedef boost::shared_ptr<OnMetaData> OnMetaData_ptr;
}  // namespace rtmp_protocol

#endif /* ONMETADATA2_HPP_ */
