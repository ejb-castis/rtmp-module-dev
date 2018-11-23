#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include <string>
#include <queue>
#include <boost/shared_ptr.hpp>

namespace rtmp_network {

class Message
{
public:
  Message() {}
  virtual ~Message() {}
  virtual std::string get_class_name() = 0;
  // return value : writed bytes count in stream
  virtual size_t write_payload(std::ostream& stream) = 0;
  virtual std::string to_string() = 0;
};
typedef boost::shared_ptr<Message> Message_ptr;
} // namespace rtmp_network
#endif /* MESSAGE_HPP_ */
