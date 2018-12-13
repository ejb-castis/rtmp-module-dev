#pragma once

#include "MediaMessage.hpp"
#include "flv_message.h"

namespace rtmp_logic {
  void publish_to_streamer(castis::streamer::media_publish_es_context_ptr& context, rtmp_protocol::MediaMessage_ptr& request);
}