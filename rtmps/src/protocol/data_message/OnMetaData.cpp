#include "OnMetaData.hpp"
#include "../../common/StreamUtil.hpp"
#include "../AMF0Serializer.hpp"

namespace rtmp_protocol {

size_t OnMetaData::write_body_payload(std::ostream& stream) {
  size_t writed = 0;
  AMF0Serializer amf0;
  std::string command(DATA_MSG_TYPE_STR_ONMETADATA);
  amf0.write_short_string(stream, command, true, writed);
  //writed += 1 + 2 + command.length();

  size_t data_writed = data_.write_payload(stream);
  writed += data_writed;

  return writed;
}

size_t MediaMetaData::write_payload(std::ostream& stream) {
  switch (data_type_) {
    case MediaMetaDataType::SIMPLE:
      return write_payload_simple(stream);
    case MediaMetaDataType::DETAIL:
      return write_payload_detail(stream);
  }
}

size_t MediaMetaData::write_payload_simple(std::ostream& stream) {
  size_t writed = 0;
  AMF0Serializer amf0;
  std::string width(ONMETADATA_STR_WIDTH);
  std::string height(ONMETADATA_STR_HEIGHT);
  std::string server(ONMETADATA_STR_SERVER);
  std::string bandwidth(ONMETADATA_STR_BANDWIDTH);

  amf0.write_type(stream, AMF0Type::AMF0_OBJECT, writed);
  amf0.write_object_key(stream, width, writed);
  amf0.write_number(stream, width_, true, writed);
  amf0.write_object_key(stream, height, writed);
  amf0.write_number(stream, height_, true, writed);
  amf0.write_object_key(stream, server, writed);
  amf0.write_short_string(stream, server_, true, writed);
  amf0.write_object_key(stream, bandwidth, writed);
  amf0.write_number(stream, bandwidth_, true, writed);
  amf0.write_end_of_object(stream, writed);

  // writed += 1;
  // writed += 2 + width.length();
  // writed += 1 + 8;
  // writed += 2 + height.length();
  // writed += 1 + 8;
  // writed += 2 + server.length();
  // writed += 1 + 2 + server_.length();
  // writed += 2 + bandwidth.length();
  // writed += 1 + 8;
  // writed += 3;

  return writed;
}

size_t MediaMetaData::write_payload_detail(std::ostream& stream) {
  size_t writed = 0;
  AMF0Serializer amf0;
  std::string description(ONMETADATA_STR_DESCRIPTION);
  std::string width(ONMETADATA_STR_WIDTH);
  std::string height(ONMETADATA_STR_HEIGHT);
  std::string framerate(ONMETADATA_STR_FRAMERATE);
  std::string video_codec_id(ONMETADATA_STR_VIDEOCODECID);
  std::string audio_codec_id(ONMETADATA_STR_AUDIOCODECID);
  std::string creation_date(ONMETADATA_STR_CREATIONDATE);
  std::string audio_channels(ONMETADATA_STR_AUDIOCHANNELS);
  std::string audio_data_rate(ONMETADATA_STR_AUDIODATARATE);
  std::string audio_device(ONMETADATA_STR_AUDIODEVICE);
  std::string audio_input_volume(ONMETADATA_STR_AUDIOINPUTVOLUME);
  std::string audio_sample_rate(ONMETADATA_STR_AUDIOSAMPLERATE);
  std::string author(ONMETADATA_STR_AUTHOR);
  std::string avc_level(ONMETADATA_STR_AVCLEVEL);
  std::string avc_profile(ONMETADATA_STR_AVCPROFILE);
  std::string copyright(ONMETADATA_STR_COPYRIGHT);
  std::string keywords(ONMETADATA_STR_KEYWORDS);
  std::string preset_name(ONMETADATA_STR_PRESETNAME);
  std::string rating(ONMETADATA_STR_RATING);
  std::string title(ONMETADATA_STR_TITLE);
  std::string video_data_rate(ONMETADATA_STR_VIDEODATARATE);
  std::string video_device(ONMETADATA_STR_VIDEODEVICE);
  std::string video_key_frame_frequency(ONMETADATA_STR_VIDEOKEYFRAME);

  amf0.write_type(stream, AMF0Type::AMF0_OBJECT, writed);

  //writed += 1;

  amf0.write_object_key(stream, description, writed);
  amf0.write_short_string(stream, description_, true, writed);

  // writed += 2 + description.length();
  // writed += 1 + 2 + description_.length();

  amf0.write_object_key(stream, width, writed);
  amf0.write_number(stream, width_, true, writed);
  amf0.write_object_key(stream, height, writed);
  amf0.write_number(stream, height_, true, writed);
  amf0.write_object_key(stream, framerate, writed);
  amf0.write_number(stream, framerate_, true, writed);

  // writed += 2 + width.length();
  // writed += 1 + 8;
  // writed += 2 + height.length();
  // writed += 1 + 8;
  // writed += 2 + framerate.length();
  // writed += 1 + 8;

  amf0.write_object_key(stream, video_codec_id, writed);
  amf0.write_short_string(stream, video_codec_id_, true, writed);
  amf0.write_object_key(stream, audio_codec_id, writed);
  amf0.write_short_string(stream, audio_codec_id_, true, writed);
  amf0.write_object_key(stream, creation_date, writed);
  amf0.write_short_string(stream, creation_date_, true, writed);

  // writed += 2 + video_codec_id.length();
  // writed += 1 + 2 + video_codec_id_.length();
  // writed += 2 + audio_codec_id.length();
  // writed += 1 + 2 + audio_codec_id_.length();
  // writed += 2 + creation_date.length();
  // writed += 1 + 2 + creation_date_.length();

  amf0.write_object_key(stream, audio_channels, writed);
  amf0.write_number(stream, audio_channels_, true, writed);
  amf0.write_object_key(stream, audio_data_rate, writed);
  amf0.write_number(stream, audio_data_rate_, true, writed);

  // writed += 2 + audio_channels.length();
  // writed += 1 + 8;
  // writed += 2 + audio_data_rate.length();
  // writed += 1 + 8;

  amf0.write_object_key(stream, audio_device, writed);
  amf0.write_short_string(stream, audio_device_, true, writed);

  // writed += 2 + audio_device.length();
  // writed += 1 + 2 + audio_device_.length();

  amf0.write_object_key(stream, audio_input_volume, writed);
  amf0.write_number(stream, audio_input_volume_, true, writed);
  amf0.write_object_key(stream, audio_sample_rate, writed);
  amf0.write_number(stream, audio_sample_rate_, true, writed);

  // writed += 2 + audio_input_volume.length();
  // writed += 1 + 8;
  // writed += 2 + audio_sample_rate.length();
  // writed += 1 + 8;

  amf0.write_object_key(stream, author, writed);
  amf0.write_short_string(stream, author_, true, writed);

  // writed += 2 + author.length();
  // writed += 1 + 2 + author_.length();

  amf0.write_object_key(stream, avc_level, writed);
  amf0.write_number(stream, avc_level_, true, writed);
  amf0.write_object_key(stream, avc_profile, writed);
  amf0.write_number(stream, avc_profile_, true, writed);

  // writed += 2 + avc_level.length();
  // writed += 1 + 8;
  // writed += 2 + avc_profile.length();
  // writed += 1 + 8;

  amf0.write_object_key(stream, copyright, writed);
  amf0.write_short_string(stream, copyright_, true, writed);
  amf0.write_object_key(stream, keywords, writed);
  amf0.write_short_string(stream, keywords_, true, writed);
  amf0.write_object_key(stream, preset_name, writed);
  amf0.write_short_string(stream, preset_name_, true, writed);
  amf0.write_object_key(stream, rating, writed);
  amf0.write_short_string(stream, rating_, true, writed);
  amf0.write_object_key(stream, title, writed);
  amf0.write_short_string(stream, title_, true, writed);

  // writed += 2 + copyright.length();
  // writed += 1 + 2 + copyright_.length();
  // writed += 2 + keywords.length();
  // writed += 1 + 2 + keywords_.length();
  // writed += 2 + preset_name.length();
  // writed += 1 + 2 + preset_name_.length();
  // writed += 2 + rating.length();
  // writed += 1 + 2 + rating_.length();
  // writed += 2 + title.length();
  // writed += 1 + 2 + title_.length();

  amf0.write_object_key(stream, video_data_rate, writed);
  amf0.write_number(stream, video_data_rate_, true, writed);

  // writed += 2 + video_data_rate.length();
  // writed += 1 + 8;

  amf0.write_object_key(stream, video_device, writed);
  amf0.write_short_string(stream, video_device_, true, writed);

  // writed += 2 + video_device.length();
  // writed += 1 + 2 + video_device_.length();

  amf0.write_object_key(stream, video_key_frame_frequency, writed);
  amf0.write_number(stream, video_key_frame_frequency_, true, writed);

  // writed += 2 + video_key_frame_frequency.length();
  // writed += 1 + 8;

  amf0.write_end_of_object(stream, writed);

  //writed += 3;

  return writed;
}

}  //  namespace rtmp_protocol

