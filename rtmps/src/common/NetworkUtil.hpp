#ifndef NETWORKUTIL_HPP_
#define NETWORKUTIL_HPP_

namespace network_util {

unsigned long long hton_8(unsigned long long value);
unsigned long hton_4(unsigned long value);
unsigned long hton_3(unsigned long value);
unsigned short hton_2(unsigned short value);

unsigned long long ntoh_8(unsigned long long value);
unsigned long ntoh_4(unsigned long value);
unsigned long ntoh_3(unsigned long value);
unsigned short ntoh_2(unsigned short value);



} // namespace network_util


#endif /* NETWORKUTIL_HPP_ */
