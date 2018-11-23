#include "TimeUtil.hpp"
 #include "boost/date_time/local_time/local_time.hpp"

namespace time_util {

long long cur_time_since_epoch() {
  boost::posix_time::ptime time_t_epoch(boost::gregorian::date(1970,1,1));
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
  boost::posix_time::time_duration diff = now - time_t_epoch;
  return diff.total_milliseconds();
}

}  // namespace time_util
