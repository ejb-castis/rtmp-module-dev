#pragma once

#include "../protocol/media_message/MediaMessage.hpp"

namespace rtmp_logic {

void write_flv_header();
void write_flv(rtmp_protocol::MediaMessage_ptr& request);

}