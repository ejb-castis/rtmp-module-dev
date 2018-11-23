#ifndef RTMPSTREAM_HPP_
#define RTMPSTREAM_HPP_

#include <boost/shared_ptr.hpp>
#include "../network/MessageSender.hpp"
#include <list>

namespace rtmp_logic {
class RtmpStream;
typedef boost::shared_ptr<RtmpStream> RtmpStream_ptr;
class RtmpStream {
 public:
  RtmpStream() {
  }
  virtual ~RtmpStream() {
  }

  unsigned int msg_stream_id_;
  std::string stream_full_name_;
  std::string stream_simple_name_;
  unsigned int chunk_stream_id_;
  unsigned int chunk_size_;

  virtual std::string get_class_name() = 0;
  bool is_same(const RtmpStream_ptr ptr) {
      return stream_full_name_.compare(ptr->stream_full_name_) == 0
          || stream_simple_name_.compare(ptr->stream_simple_name_) == 0;
    }

  bool is_same_name(const std::string& stream_name) {
      return stream_full_name_.compare(stream_name) == 0
          || stream_simple_name_.compare(stream_name) == 0;
    }
};

class OutRtmpStream;
typedef boost::shared_ptr<OutRtmpStream> OutRtmpStream_ptr;
class OutRtmpStream : public RtmpStream {
 public:
  OutRtmpStream(rtmp_network::MessageSender_wptr sender)
      : sender_(sender) {
  }
  virtual ~OutRtmpStream() {
  }
  void write_message(rtmp_network::Message_ptr msg) {
    sender_->push_to_send_queue(msg);
  }
  virtual std::string get_class_name() {
    return "OutRtmpStream";
  }

 private:
  rtmp_network::MessageSender_wptr sender_;
};

class InRtmpStream : public RtmpStream {
 public:
  InRtmpStream() {
  }
  virtual ~InRtmpStream() {
  }
  void add_data(rtmp_network::Message_ptr msg);
  void register_link(OutRtmpStream_ptr out);
  void unregister_link(OutRtmpStream_ptr out);
  virtual std::string get_class_name() {
    return "InRtmpStream";
  }
  bool exists_in_linked_out_streams(OutRtmpStream_ptr out);
 private:
  std::list<OutRtmpStream_ptr> linked_out_streams_;
};
typedef boost::shared_ptr<InRtmpStream> InRtmpStream_ptr;

class EmptyRtmpStream : public RtmpStream {
 public:
  EmptyRtmpStream() {
  }
  virtual ~EmptyRtmpStream() {
  }
  virtual std::string get_class_name() {
    return "EmptyRtmpStream";
  }
};
typedef boost::shared_ptr<EmptyRtmpStream> EmptyRtmpStream_ptr;

template<class T>
class SameStream : public std::unary_function<T, bool> {
 private:
  std::string name_;

 public:
  SameStream(const std::string& name)
      : name_(name) {
  }
  bool operator()(const T& stream) {
    return name_.compare(stream->stream_full_name_)
        || name_.compare(stream->stream_simple_name_);
  }
};

}  // namespace rtmp_logic

#endif /* RTMPSTREAM_HPP_ */
