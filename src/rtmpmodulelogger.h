#pragma once
#include "cichannellogger.h"

#define RTMPLOG(severity) \
  CILOG_CHANNEL("RTMP", severity)

#define RTMPLOGF(severity, format, ...) \
  CILOGF_CHANNEL("RTMP", severity, format, ##__VA_ARGS__)

namespace castis {
namespace logger {

inline boost::shared_ptr<cichannellog_async_sink>
init_rtmpmodule_logger(const std::string& app_name,
                         const std::string& app_version,
                         const std::string& target = "./log",
                         int64_t rotation_size = 100 * 100 * 1024
                         ) {
    return init_async_channel_logger(app_name, app_version, "RTMP", target, rotation_size, true);
}
inline boost::shared_ptr<cichannellog_date_hour_async_sink>
init_rtmpmodule_date_hour_logger(const std::string& app_name,
                         const std::string& app_version,
                         const std::string& target = "./log",
                         const std::string& file_name_prefix_format="%Y-%m-%d[%H]"
                         ) {
    return init_async_date_hour_channel_logger(app_name, app_version, "RTMP", target, file_name_prefix_format, true);
}
}  
}