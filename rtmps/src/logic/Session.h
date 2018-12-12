#pragma once
#include <map>
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>
#include "../protocol/RtmpHeader.hpp"
#include "../../../src/rtmpmodulelogger.h"
#include <memory>
#include <algorithm>
#include <sstream>

namespace rtmp {

const std::size_t kChunkSize = 128;
const unsigned kMagicNumber = 0;

class Session;
class ChunkStream;
class MessageStream;
class Message;
class ChunkData;

typedef boost::shared_ptr<rtmp_protocol::RtmpHeader> rtmpheader_ptr;
typedef boost::shared_ptr<Session> session_ptr;
typedef boost::shared_ptr<ChunkStream> chunkstream_ptr;
typedef boost::shared_ptr<MessageStream> messagestream_ptr;
typedef boost::shared_ptr<Message> message_ptr;
typedef boost::shared_ptr<ChunkData> chunkdata_ptr;
typedef boost::shared_ptr<unsigned char> char_ptr;

class ChunkData{
public:
  std::vector<unsigned char> data_;
};

class Message {
public:
  std::size_t id_{0};
  unsigned timestamp_{0};
  std::size_t length_{0};
  unsigned type_id_{0};
  std::size_t received_length_{0};
  unsigned chunk_stream_id_{0};
  unsigned message_stream_id_{0};


  //char_ptr chunk_data_;
  unsigned char* chunk_data_{nullptr};
  ~Message() { 
    //RTMPLOGF(debug, "Message[%1%],chunk_data_[%2%]", this, chunk_data_);
    if (chunk_data_) delete []chunk_data_; 
  }
  void set_id(unsigned id) { id_ = id; }
  std::string to_string() { 
    std::ostringstream oss;
    oss << "id[" << id_ << "],";
    oss << "chunk_stream_id[" << chunk_stream_id_ << "],";
    oss << "message_stream_id[" << message_stream_id_ << "],";
    oss << "timestamp[" << timestamp_ << "],";
    oss << "length[" << length_ << "],";
    oss << "type_id[" << type_id_ << "],";
    oss << "received_length[" << received_length_ << "]";
    return oss.str();
  }    
   std::list<chunkdata_ptr>chunk_datas_;
};

class MessageStream {
public:  
  unsigned id_;
  message_ptr message_;

  ~MessageStream() {
  }
  message_ptr new_message() { 
    static unsigned uid{0};

    message_ = message_ptr(new Message); 
    message_->set_id(++uid);
    return message_; 
  }
  message_ptr get_current_message() { return message_; };
  void set_current_message(message_ptr message) { message_ = message; };
  void set_id(unsigned id) { id_ = id; }
};

class ChunkStream {
public:  
  unsigned id_;
  std::map<unsigned, messagestream_ptr> message_streams_;
  rtmpheader_ptr preceding_chunk_header_;

  void set_id(unsigned id) { id_ = id; }
  rtmpheader_ptr preceding_chunk_header() {
    return preceding_chunk_header_;
  }
  void set_preceding_chunk_header(rtmpheader_ptr preceding_header) {
    preceding_chunk_header_= preceding_header;
  }
  std::string to_string() { 
    std::ostringstream oss;
    oss << "[" << this << "],";
    oss << "id_[" << id_ << "]";
    return oss.str();
  }    

  messagestream_ptr get_message_stream(unsigned message_stream_id) { 
    auto it = message_streams_.find(message_stream_id);
    if (it != message_streams_.end()) 
    {
      return message_streams_[message_stream_id];
    }
    messagestream_ptr message_stream= messagestream_ptr(new MessageStream);
    message_stream->set_id(message_stream_id);
    message_streams_[message_stream_id] = message_stream;
    return message_stream;    
  }
};

class Session {
public:
  unsigned id_;
  std::map<unsigned, chunkstream_ptr> chunk_streams_;
  std::size_t chunk_size_{kChunkSize};

  void set_id(unsigned id) { id_ = id; }
  chunkstream_ptr get_chunk_stream(unsigned chunk_stream_id) { 
    auto it = chunk_streams_.find(chunk_stream_id);
    if (it != chunk_streams_.end()) 
    {
      return chunk_streams_[chunk_stream_id];
    }
    chunkstream_ptr chunkstream= chunkstream_ptr(new ChunkStream);
    chunkstream->set_id(chunk_stream_id);
    chunk_streams_[chunk_stream_id] = chunkstream;
    return chunkstream;
  }

};

static std::map<unsigned, session_ptr> sessions_;
inline void remove_session(std::map<unsigned, session_ptr> sessions, unsigned id) {
  auto it = sessions.find(id);
  if (it != sessions.end()) 
    sessions.erase(id);
}
inline session_ptr get_session(std::map<unsigned, session_ptr> sessions, unsigned id) {
  auto it = sessions.find(id);
  if (it != sessions.end()) 
  {
    return sessions[id];
  }
  session_ptr session= session_ptr(new Session);
  session->set_id(id);

  sessions[id] = session;
  return session;
}

} // rtmp