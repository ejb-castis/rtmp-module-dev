#include "write_flv.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/bind/bind.hpp>
#include "../../../src/rtmpmodulelogger.h"

namespace rtmp_logic {

using namespace rtmp_protocol;

void write_flv_header() {
  std::string flv_file_name = "dump.flv";
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

void write_flv(MediaMessage_ptr& request) {
  static int audio_index = 0;
  static int video_index = 0;

  static bool write_audio_header_done = false;
  static bool write_video_header_done = false;
  static bool write_flv_header_done = false;
  static uint32_t video_timestamp_delta = 0;
  static uint32_t audio_timestamp_delta = 0;

  //StreamManager& mgr = StreamManager::get_instance();

  auto publish_type = PublishType::record;
  switch(publish_type) {
    case PublishType::record: {

      if (write_flv_header_done == false) {
          write_flv_header_done = true;    

          write_flv_header();
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

        std::string flv_file_name = "dump.flv";
        std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );

        // write tag header
        flvfile.write(reinterpret_cast<const char*>(hdr), ph-hdr);

        // write content        
        flvfile.write(reinterpret_cast<const char*>(request->get_data().get()), request->get_data_len());

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

          write_flv_header();  
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

        std::string flv_file_name = "dump.flv";
        std::ofstream flvfile(flv_file_name, std::ios::binary | std::ofstream::out | std::ofstream::app );

        // write tag header
        flvfile.write(reinterpret_cast<const char*>(hdr), ph-hdr);

        // write content
        flvfile.write(reinterpret_cast<const char*>(request->get_data().get()), request->get_data_len());

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