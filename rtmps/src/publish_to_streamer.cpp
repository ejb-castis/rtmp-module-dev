#include <chrono>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <string>

#include "rtmpmodulelogger.h"

#include "flv_message.h"

// #include "HTTPAsyncClient.h"
// #include "rapidjson/document.h"
// #include "rapidjson/error/en.h"
// #include "rapidjson/stringbuffer.h"
// #include "rapidjson/writer.h"
// #include "streamer/base64.h"


#include "publish_to_streamer.h"

namespace rtmp_logic {

void publish_to_streamer(castis::streamer::media_publish_es_context_ptr& context, 
  rtmp_protocol::MediaMessage_ptr& request) {
  RTMPLOGF(debug,"pulishing stream_name[%1%],client_id[%2%]",context->stream_name_, context->client_id_);
}

} // namespace rtmp_logic