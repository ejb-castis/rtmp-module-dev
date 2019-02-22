#pragma once

#include "MediaMessage.hpp"
#include "flv_message.h"

namespace rtmp_logic {
void write_flv(castis::streamer::media_publish_es_context_ptr context,
               rtmp_protocol::MediaMessage_ptr& request);
void write_flv_media_message_dump(
    castis::streamer::media_publish_es_context_ptr context,
    rtmp_protocol::MediaMessage_ptr& request);
}  // namespace rtmp_logic