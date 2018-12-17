#include "write_flv.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/bind/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

#include "../../../src/rtmpmodulelogger.h"

namespace rtmp_logic {

using namespace rtmp_protocol;

void write_4byte(uint32_t value, char*& dst) {
    *dst = (value & 0xff000000) >> 24; ++dst;
    *dst = (value & 0x00ff0000) >> 16; ++dst;
    *dst = (value & 0x0000ff00) >>  8; ++dst;
    *dst = (value & 0x000000ff)      ; ++dst;
}
void write_1byte(uint8_t value, char*& dst) {
    *dst = value; ++dst;
}

void write_flv_header(const std::string& flv_file_name) {

  std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );

  static u_char flv_header[] = {
      0x46, /* 'F' */
      0x4c, /* 'L' */
      0x56, /* 'V' */
      0x01, /* version = 1 */
      0x05, // 0x05, /* 00000101 = has audio & video */
      0x00,
      0x00,
      0x00,
      0x09, /* header size */
      0x00,
      0x00,
      0x00,
      0x00  /* PreviousTagSize0 (not actually a header) */
  };
  size_t size=sizeof(flv_header);
  flvfile.write(reinterpret_cast<const char*>(flv_header), size);
  flvfile.flush();
  flvfile.close();
}

void write_flv_audio_dump(const std::string& file_name, unsigned& audio_index, const char* data, std::size_t length) {
  ++audio_index;
  std::string flv_file_name = file_name + "." + std::to_string(audio_index);
  std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );

  flvfile.write(data, length);
  flvfile.flush();
  flvfile.close();  
}
void write_flv_video_dump(const std::string& file_name, unsigned& video_index, const char* data, std::size_t length) {
  ++video_index;
  std::string flv_file_name = file_name + "." + std::to_string(video_index);
  std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );

  flvfile.write(data, length);
  flvfile.flush();
  flvfile.close();
}

void write_flv_media_dump(castis::streamer::media_publish_es_context_ptr& context, MediaMessage_ptr& request) {
  unsigned int& media_index = context->record_flv_context_.media_index;

  const char* data =  reinterpret_cast<const char*>(request->get_data().get());
  std::size_t data_length = request->get_data_len();

  char header[9];
  char* header_ptr = header;
  std::size_t header_length = 1 + 4 + 4;

  write_1byte(request->get_header()->msg_type_id_, header_ptr);  
  write_4byte(request->get_header()->timestamp_, header_ptr);  
  write_4byte(request->get_header()->msg_length_, header_ptr);  

  std::string stream_name = context->stream_name_;
  std::string client_id = context->client_id_;
  
  if (stream_name.empty()) stream_name="anonymous";
  boost::filesystem::path file_path("dump/" + stream_name + "/media");
  boost::filesystem::create_directories(file_path);
  ++media_index;

  std::string flv_file_name = file_path.string() + "/" + stream_name + ".dump." + std::to_string(media_index);
  std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );

  flvfile.write(header, header_length);
  flvfile.write(data, data_length);
  flvfile.flush();
  flvfile.close();  

  if(request->get_header()->is_audio_msg()) {
    boost::filesystem::path file_path("dump/" + stream_name + "/audio_messag_header");
    boost::filesystem::create_directories(file_path);
    std::string flv_file_name = file_path.string() + "/" + stream_name + ".audio.header." + std::to_string(context->record_flv_context_.audio_index+1);
    std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );
    flvfile.write(header, header_length);
    flvfile.flush();
    flvfile.close();  
  } else {
    boost::filesystem::path file_path("dump/" + stream_name + "/video_messag_header");
    boost::filesystem::create_directories(file_path);
    std::string flv_file_name = file_path.string() + "/" + stream_name + ".video.header." + std::to_string(context->record_flv_context_.video_index+1);
    std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );
    flvfile.write(header, header_length);
    flvfile.flush();
    flvfile.close();  
  }


}

void write_flv(castis::streamer::media_publish_es_context_ptr context, MediaMessage_ptr& request) {
  
  bool& write_audio_header_done = context->record_flv_context_.write_audio_header_done;
  bool& write_video_header_done = context->record_flv_context_.write_audio_header_done;
  bool& write_flv_header_done = context->record_flv_context_.write_flv_header_done;
  uint32_t video_timestamp_delta = context->video_timestamp_delta_;
  uint32_t audio_timestamp_delta = context->audio_timestamp_delta_;

  write_flv_media_dump(context, request);
  std::string stream_name = context->stream_name_;
  if (stream_name.empty()) stream_name="anonymous";

  auto publish_type = PublishType::record;
  switch(publish_type) {
    case PublishType::record: {

      boost::filesystem::path dump_file_path("dump/"+stream_name);
      boost::filesystem::create_directories(dump_file_path);
      
      boost::filesystem::path audio_dump_file_path = dump_file_path / "audio";
      boost::filesystem::create_directories(audio_dump_file_path);

      boost::filesystem::path video_dump_file_path = dump_file_path / "video";
      boost::filesystem::create_directories(video_dump_file_path);

      std::string flv_file_name = dump_file_path.string() +"/"+  stream_name + ".flv";

      if (write_flv_header_done == false) {
          write_flv_header_done = true;    

          write_flv_header(flv_file_name);   
        }


      if (request->get_header()->is_audio_msg()) {

        if (write_audio_header_done == false) {
          write_audio_header_done = true;    
        }

        // refer to adobe's flv tag definition and etc.
        u_char hdr[11], *p, *ph;
        uint32_t timestamp_in_mill, tag_size;

        // timestamp value of first tag MUST be zero, 
        // otherwise all timestamp value should be shift some delta.
        timestamp_in_mill = request->get_header()->timestamp_;
        timestamp_in_mill = timestamp_in_mill + audio_timestamp_delta;

        ph = hdr;
        
        // first 2 bits are set to 0 implicitly
        // last 
        *ph++ = (u_char)request->get_header()->msg_type_id_;

        // little endian to big endian
        p = (u_char*)&request->get_header()->msg_length_;
        *ph++ = p[2];
        *ph++ = p[1];
        *ph++ = p[0];
         
        // assumption: rtmp timestamp value is in millsec
        //timestamp : 24bit, extended : 8bit
        
        // lower 24bit of timestamp
        // little endian to big endian
        p = (u_char*)&timestamp_in_mill;
        *ph++ = p[2];
        *ph++ = p[1];
        *ph++ = p[0];
        // extended : upper 8bit of timestamp
        *ph++ = p[3];

        // stream id
        *ph++ = 0;
        *ph++ = 0;
        *ph++ = 0;

        // tag size = header size + data size
        tag_size = (ph - hdr) + request->get_header()->msg_length_;

        std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );

        // write tag header
        flvfile.write(reinterpret_cast<const char*>(hdr), ph-hdr);

        // write content        
        flvfile.write(reinterpret_cast<const char*>(request->get_data().get()), request->get_data_len());

        std::string audio_flv_file_name = audio_dump_file_path.string() +"/"+  stream_name + ".dump";

        write_flv_audio_dump(audio_flv_file_name, context->record_flv_context_.audio_index, reinterpret_cast<const char*>(request->get_data().get()), request->get_data_len());

        // write tag footer
        ph = hdr;
        p = (u_char*)&tag_size;

        *ph++ = p[3];
        *ph++ = p[2];
        *ph++ = p[1];
        *ph++ = p[0];
        flvfile.write(reinterpret_cast<const char*>(hdr), ph-hdr);

        flvfile.flush();
        flvfile.close();

        


      } else if (request->get_header()->is_video_msg()) {

        if (write_flv_header_done == false) {
          write_flv_header_done = true;    

          write_flv_header(flv_file_name);
        }

        if (write_video_header_done == false) {
          write_video_header_done = true;    
        }

        // from nginx
        u_char hdr[11], *p, *ph;
        uint32_t timestamp_in_mill, tag_size;
        timestamp_in_mill = request->get_header()->timestamp_;
        timestamp_in_mill = timestamp_in_mill + video_timestamp_delta;

        ph = hdr;

        *ph++ = (u_char)request->get_header()->msg_type_id_;
        p = (u_char*)&request->get_header()->msg_length_;
        *ph++ = p[2];
        *ph++ = p[1];
        *ph++ = p[0];

        p = (u_char*)&timestamp_in_mill;
        *ph++ = p[2];
        *ph++ = p[1];
        *ph++ = p[0];
        *ph++ = p[3];

        *ph++ = 0;
        *ph++ = 0;
        *ph++ = 0;

        tag_size = (ph - hdr) + request->get_header()->msg_length_;

        std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );

        // write tag header
        flvfile.write(reinterpret_cast<const char*>(hdr), ph-hdr);

        // write content
        flvfile.write(reinterpret_cast<const char*>(request->get_data().get()), request->get_data_len());

        std::string video_flv_file_name = video_dump_file_path.string() +"/"+  stream_name + ".dump";
        write_flv_video_dump(video_flv_file_name, context->record_flv_context_.video_index, reinterpret_cast<const char*>(request->get_data().get()), request->get_data_len());

        ph = hdr;
        p = (u_char*)&tag_size;
        *ph++ = p[3];
        *ph++ = p[2];
        *ph++ = p[1];
        *ph++ = p[0];
        
        // write tag footer
        flvfile.write(reinterpret_cast<const char*>(hdr), ph-hdr);

        flvfile.flush();
        flvfile.close();

       } else {
        RTMPLOG(warning) << "NOT support handle other type data";
        return;
      }
    }break;
    case PublishType::append:{

    }break;
    case PublishType::live:{

    }break;
  }
}





} // end of rtmp_logic

