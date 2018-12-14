#pragma once

#include "MediaMessage.hpp"
#include "flv_message.h"

namespace rtmp_logic {

void write_flv_audio_dump(const char*, unsigned int&, std::size_t length);
void write_flv_video_dump(const char*, unsigned int&, std::size_t length);
void write_flv_header(const std::string&);
void write_flv(castis::streamer::media_publish_es_context_ptr context, rtmp_protocol::MediaMessage_ptr& request);

}