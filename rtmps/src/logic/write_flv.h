#pragma once

#include "../protocol/media_message/MediaMessage.hpp"

namespace rtmp_logic {

void write_flv_audio_dump(const char*, std::size_t length);
void write_flv_video_dump(const char*, std::size_t length);
void write_flv_header(const std::string&);
void write_flv(rtmp_protocol::MediaMessage_ptr& request);

}