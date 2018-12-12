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
  
  boost::filesystem::path file_path("dump");
  boost::filesystem::create_directories(file_path);

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

void write_flv_audio_dump(const char* data, std::size_t length) {
  static int audio_index = 0;
  ++audio_index;

  boost::filesystem::path file_path("dump/audio");
  boost::filesystem::create_directories(file_path);

  std::string flv_file_name = "dump/audio/audio.dump." + std::to_string(audio_index);
  std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );

  flvfile.write(data, length);
  flvfile.flush();
  flvfile.close();  
}
void write_flv_video_dump(const char* data, std::size_t length) {
  static int video_index = 0;

  boost::filesystem::path file_path("dump/video");
  boost::filesystem::create_directories(file_path);

  ++video_index;
  std::string flv_file_name = "dump/video/video.dump." + std::to_string(video_index);
  std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );

  flvfile.write(data, length);
  flvfile.flush();
  flvfile.close();
}

void write_flv_media_dump(MediaMessage_ptr& request) {
  static unsigned int media_index = 0;

  const char* data =  reinterpret_cast<const char*>(request->get_data().get());
  std::size_t data_length = request->get_data_len();

  char header[9];
  char* header_ptr = header;
  std::size_t header_length = 1 + 4 + 4;

  write_1byte(request->get_header()->msg_type_id_, header_ptr);  
  write_4byte(request->get_header()->timestamp_, header_ptr);  
  write_4byte(request->get_header()->msg_length_, header_ptr);  

  boost::filesystem::path file_path("dump/flv_es");
  boost::filesystem::create_directories(file_path);
  ++media_index;

  std::string flv_file_name = "dump/flv_es/flv_es.dump." + std::to_string(media_index);
  std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );

  flvfile.write(header, header_length);
  flvfile.write(data, data_length);
  flvfile.flush();
  flvfile.close();  
}

void write_flv(MediaMessage_ptr& request) {
  
  static bool write_audio_header_done = false;
  static bool write_video_header_done = false;
  static bool write_flv_header_done = false;
  static uint32_t video_timestamp_delta = 0;
  static uint32_t audio_timestamp_delta = 0;

  write_flv_media_dump(request);

  auto publish_type = PublishType::record;
  switch(publish_type) {
    case PublishType::record: {

      std::string flv_file_name = "dump/dump.flv";

      if (write_flv_header_done == false) {
          write_flv_header_done = true;    

          write_flv_header(flv_file_name);   
        }


      if (request->get_header()->is_audio_msg()) {

        if (write_audio_header_done == false) {
          write_audio_header_done = true;    

          audio_timestamp_delta = 0; // 0 - first timestamp of the first audio message 
        }

        // refer to adobe's flv tag definition and nginx rtmp module
        u_char                      hdr[11], *p, *ph;
        uint32_t                    timestamp_in_mill, tag_size;

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

        write_flv_audio_dump(reinterpret_cast<const char*>(request->get_data().get()), request->get_data_len());

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

          video_timestamp_delta = 0; // 0 - first timestamp of the first video message 
        }

        // from nginx
        u_char                      hdr[11], *p, *ph;
        uint32_t                    timestamp_in_mill, tag_size;
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

        write_flv_video_dump(reinterpret_cast<const char*>(request->get_data().get()), request->get_data_len());

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

