#include "NetworkUtil.hpp"
#include <boost/asio/detail/socket_ops.hpp>

namespace network_util {
unsigned long long hton_8(unsigned long long value) {
  if(1 == hton_4(1))
    return value;

  unsigned long long rstamp = 0x0000000000000000;
  rstamp |= (value & 0xff00000000000000) >> 56;
  rstamp |= (value & 0x00ff000000000000) >> 40;
  rstamp |= (value & 0x0000ff0000000000) >> 24;
  rstamp |= (value & 0x000000ff00000000) >> 8;
  rstamp |= (value & 0x00000000ff000000) << 8;
  rstamp |= (value & 0x0000000000ff0000) << 24;
  rstamp |= (value & 0x000000000000ff00) << 40;
  rstamp |= (value & 0x00000000000000ff) << 56;
  return rstamp;
}
unsigned long hton_4(unsigned long value) {
  return boost::asio::detail::socket_ops::host_to_network_long(value);
}
unsigned long hton_3(unsigned long value) {
  if(1 == hton_4(1))
      return value;

  return hton_4(value) >> 8;
}
unsigned short hton_2(unsigned short value) {
  return boost::asio::detail::socket_ops::host_to_network_short(value);
}

unsigned long long ntoh_8(unsigned long long value) {
  if(1 == ntoh_4(1))
    return value;

  unsigned long long rstamp = 0x0000000000000000;
  rstamp |= (value & 0xff00000000000000) >> 56;
  rstamp |= (value & 0x00ff000000000000) >> 40;
  rstamp |= (value & 0x0000ff0000000000) >> 24;
  rstamp |= (value & 0x000000ff00000000) >> 8;
  rstamp |= (value & 0x00000000ff000000) << 8;
  rstamp |= (value & 0x0000000000ff0000) << 24;
  rstamp |= (value & 0x000000000000ff00) << 40;
  rstamp |= (value & 0x00000000000000ff) << 56;
  return rstamp;
}
unsigned long ntoh_4(unsigned long value) {
  return boost::asio::detail::socket_ops::network_to_host_long(value);
}
unsigned long ntoh_3(unsigned long value) {
  if(1 == ntoh_4(1))
      return value;

  return ntoh_4(value) >> 8;
}
unsigned short ntoh_2(unsigned short value) {
  return boost::asio::detail::socket_ops::network_to_host_short(value);
}

}  // namespace network_util
