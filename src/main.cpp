#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include "../rtmps/src/logic/RtmpHandler.hpp"
#include "../rtmps/src/network/Server.hpp"
#include "../rtmps/src/protocol/RtmpParser.hpp"
#include "rtmpmodulelogger.h"
#include "version.h"

int main() {
  auto sink = castis::logger::init_rtmpmodule_logger(
      "rtmp-module", RTMPMODULE_BRIEF_VERSION_INFO);

  RTMPLOG(info) << "rtmp-module started";

  std::shared_ptr<void> releaser(nullptr, [&sink](void*) {
    RTMPLOG(info) << "rtmp-module stopped";
    castis::logger::stop_logger(sink);
  });

  const std::size_t num_threads = 1;
  const std::string rtmp_port = "1935";
  const std::string ip = "172.16.33.52";

  std::thread t([&]() {
    try {
      rtmp_network::server s(ip, rtmp_port, num_threads,
                             rtmp_logic::RtmpHandlerFactory_ptr(
                                 new rtmp_logic::RtmpHandlerFactory),
                             rtmp_protocol::RtmpParserFactory_ptr(
                                 new rtmp_protocol::RtmpParserFactory));

      s.run();
    } catch (std::exception& e) {
      std::cerr << "exception: " << e.what() << "\n";
    }
  });

  t.join();


  return 0;
}
