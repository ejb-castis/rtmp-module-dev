#include <gtest/gtest.h>
#include <array>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include "src/flv_util.cpp"
#include "src/parsingflv.cpp"


using namespace flv_util;
using namespace flv;
using namespace parsingflv;
using namespace std;


TEST(FLV, put_test)
{
  unsigned char mp4_nal[]= {
    0x00, 0x00, 0x00, 0x01 
  };
  size_t mp4_nal_size = sizeof (mp4_nal) / sizeof(unsigned char) ;

  buffer_t ts_nal_start_code(4);
  EXPECT_TRUE ( 0 == ts_nal_start_code.len_);
  ts_nal_start_code.len_ = 4;
  put_same_nbyte(ts_nal_start_code, 0x00, 3);
  put_last_byte(ts_nal_start_code, 0x01);

  EXPECT_EQ(mp4_nal_size, ts_nal_start_code.len_);
  EXPECT_TRUE(std::equal(ts_nal_start_code.ptr_, ts_nal_start_code.ptr_ + ts_nal_start_code.len_, mp4_nal));
  
}

TEST(FLV, replace_nalu_start_code_from_mp4_to_ts)
{
  unsigned char mp4_nal[]= {
    0x00, 0x00, 0x00, 0x02,
    0xD9, 0x40,
    0x00, 0x00, 0x00, 0x01, 
    0xD9,
    0x00, 0x00, 0x00, 0x0A, 
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,

    0x00, 0x00, 0x00, 0x10, 
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
  };
  unsigned char ts_nal[]= {
    0x00, 0x00, 0x00, 0x01,
    0xD9, 0x40,
    0x00, 0x00, 0x00, 0x01, 
    0xD9,
    0x00, 0x00, 0x00, 0x01, 
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x00, 0x00, 0x00, 0x01, 
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
  };
  
  size_t mp4_nal_size = sizeof (mp4_nal) / sizeof(unsigned char) ;
  size_t ts_nal_size = sizeof (ts_nal) / sizeof(unsigned char) ;

  buffer_t mp4_buffer(mp4_nal_size);
  
  read_nbyte(mp4_nal, mp4_nal_size, mp4_buffer);

  EXPECT_EQ(mp4_nal_size, mp4_buffer.len_);
  EXPECT_TRUE(std::equal(mp4_buffer.ptr_, mp4_buffer.ptr_ + mp4_buffer.len_, mp4_nal));

  replace_nalu_start_code_from_mp4_to_ts(4, mp4_buffer);

  EXPECT_TRUE(ts_nal_size == mp4_buffer.len_);
  EXPECT_TRUE(std::equal(mp4_buffer.ptr_, mp4_buffer.ptr_ + mp4_buffer.len_, ts_nal));

}

TEST(FLV, vector_insert)
{
  std::vector<unsigned char> sps(4,4);
  std::vector<unsigned char> pps(3,3);

  std::vector<unsigned char> codec_info;

  unsigned char d[] = { 0x00,0x00,0x00,0x01 };
  codec_info.insert (codec_info.begin(), pps.begin(), pps.end());
  codec_info.insert (codec_info.begin(), d, d+4);
  codec_info.insert (codec_info.begin(), sps.begin(), sps.end());
  codec_info.insert (codec_info.begin(), d, d+4);

  //std::cout << to_hex(codec_info.data(), 15) << std::endl ;

  unsigned char data[]= {
    0x00, 0x00, 0x00, 0x01, 0x04, 0x04, 0x04, 0x04, 
    0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03 
  };

  EXPECT_TRUE(std::equal(codec_info.begin(), codec_info.end() , data ));
}

TEST(FLV, 4byte_to_vector_big_endian)
{
  uint32_t value = 128;
  std::vector<unsigned char> v = to_vector(value);
  std::vector<unsigned char> v2 = to_vector2(value);

  // big endian 128
  unsigned char data[]= {
    0x00, 0x00, 0x00, 0x80
  };

  std::cout << to_hex(v) << std::endl ;
  std::cout << to_hex(v2) << std::endl ;

  EXPECT_TRUE(std::equal(v.begin(), v.end() , data ));
  EXPECT_TRUE(std::equal(v2.begin(), v2.end() , data ));
}

TEST(FLV, 4byte_to_string_big_endian)
{
  uint32_t value = 128;
  std::string v = flv_util::to_string(value);

  std::cout << to_hex(v) << std::endl ;
  EXPECT_EQ('\x00', v[0]);
  EXPECT_EQ('\x00', v[1]);
  EXPECT_EQ('\x00', v[2]);
  EXPECT_EQ('\x80', v[3]);
}


TEST(FLV, copy_buffer_t_to_vector)
{
  buffer_t buf(5);
  buf.ptr_[0] = 1;
  buf.ptr_[1] = 2;
  buf.ptr_[2] = 3;
  buf.ptr_[3] = 4;
  buf.ptr_[4] = 5;
  buf.len_ = 5;
  std::vector<unsigned char> v(5);
  std::copy(buf.ptr_, buf.ptr_+buf.len_, v.begin());

  auto v2 = to_vector(buf);
  EXPECT_TRUE(std::equal(buf.ptr_, buf.ptr_ + buf.len_, v.begin()));
  EXPECT_TRUE(std::equal(v2.begin(), v2.end() , v.begin()));
}


TEST(FLV, to_string_buffer_t)
{
  unsigned char data[]= {
    0xaf, 0x02, 0xf8, 0x3e, 0x01, 0x1d, 0x2b, 0x20
  };

  buffer_t buffer(8);
  read_nbyte(data, 8, buffer);
  std::string ret = to_string(buffer);

  EXPECT_EQ("\xaf\x02\xf8\x3e\x01\x1d\x2b\x20", ret);
}

TEST(FLV, get_aac_header_complex4)
{
  // audio object type : 2
  // sample rate index : 15
  // sample rate : 36501 hz
  // channel config : 3
  unsigned char flv_aac_seq_header_data[]= {
    0xaf, 0x00, 0xf8, 0x3e, 0x01, 0x1d, 0x2b, 0x20
  };
  unsigned char* aac_seq_buffer= flv_aac_seq_header_data;
  std::size_t aac_seq_buffer_len = sizeof(aac_seq_buffer) / sizeof(unsigned char);
  uint32_t sample_rate;
  uint32_t channel_count;

  EXPECT_TRUE(get_aac_header(aac_seq_buffer, sample_rate, channel_count, aac_seq_buffer_len));
  EXPECT_TRUE(sample_rate == 36501);
  EXPECT_TRUE(channel_count == 9 );
}


TEST(FLV, get_aac_header_complex3)
{
  // audio object type : 2
  // sample rate index : 15
  // sample rate : 36501 hz
  // channel config : 3
  unsigned char flv_aac_seq_header_data[]= {
    0xaf, 0x00, 0xf8, 0x26, 0x40
  };
  unsigned char* aac_seq_buffer= flv_aac_seq_header_data;
  std::size_t aac_seq_buffer_len = sizeof(aac_seq_buffer) / sizeof(unsigned char);
  uint32_t sample_rate;
  uint32_t channel_count;

  EXPECT_TRUE(get_aac_header(aac_seq_buffer, sample_rate, channel_count, aac_seq_buffer_len));
  EXPECT_TRUE(48000 == sample_rate);
  EXPECT_TRUE(2 == channel_count);
}


TEST(FLV, get_aac_header_complex2)
{
  // audio object type : 2
  // sample rate index : 15
  // sample rate : 36501 hz
  // channel config : 3
  unsigned char flv_aac_seq_header_data[]= {
    0xaf, 0x00, 0x17, 0x80, 0x47, 0x4a, 0xb8
  };
  unsigned char* aac_seq_buffer= flv_aac_seq_header_data;
  std::size_t aac_seq_buffer_len = sizeof(aac_seq_buffer) / sizeof(unsigned char);
  uint32_t sample_rate;
  uint32_t channel_count;

  EXPECT_TRUE(get_aac_header(aac_seq_buffer, sample_rate, channel_count, aac_seq_buffer_len));
  EXPECT_TRUE(36501== sample_rate);
  EXPECT_TRUE(7== channel_count);
}


TEST(FLV, get_aac_header_complex)
{
  // audio object type : 2
  // sample rate index : 15
  // sample rate : 8 hz
  // channel config : 3
  unsigned char flv_aac_seq_header_data[]= {
    0xaf, 0x00, 0x17, 0x80, 0x00, 0x04, 0x18
  };
  unsigned char* aac_seq_buffer= flv_aac_seq_header_data;
  std::size_t aac_seq_buffer_len = sizeof(aac_seq_buffer) / sizeof(unsigned char);
  uint32_t sample_rate;
  uint32_t channel_count;

  EXPECT_TRUE(get_aac_header(aac_seq_buffer, sample_rate, channel_count, aac_seq_buffer_len));
  EXPECT_TRUE(8 == sample_rate);
  EXPECT_TRUE(3 == channel_count);
}


TEST(FLV, get_aac_header)
{
  unsigned char flv_aac_seq_header_data[]= {
    0xaf, 0x00, 0x11, 0x90, 0x56, 0xe5, 0x00
  };
  unsigned char* aac_seq_buffer= flv_aac_seq_header_data;
  std::size_t aac_seq_buffer_len = sizeof(aac_seq_buffer) / sizeof(unsigned char);
  uint32_t sample_rate;
  uint32_t channel_count;

  EXPECT_TRUE(get_aac_header(aac_seq_buffer, sample_rate, channel_count, aac_seq_buffer_len));
  EXPECT_TRUE(48000 == sample_rate);
  EXPECT_TRUE(2 == channel_count);

}

TEST(FLV, get_aac_header_video)
{
  unsigned char flv_aac_seq_header_data[]= {
    0xaf, 0x00, 0x11, 0x90, 0x56, 0xe5, 0x00
  };

  unsigned char flv_aac_data[] = {
    0xaf, 0x01, 0xde, 0x04, 0x00, 0x4c, 0x61, 0x76,
    0x63, 0x35, 0x37, 0x2e, 0x31, 0x30, 0x37, 0x2e,
    0x31, 0x30, 0x30
  };

  unsigned char flv_aac_audio_data[] = {
    0xde, 0x04, 0x00, 0x4c, 0x61, 0x76, 0x63, 0x35,
    0x37, 0x2e, 0x31, 0x30, 0x37, 0x2e, 0x31, 0x30,
    0x30
  };

  EXPECT_TRUE(is_aac_sequence_header(flv_aac_seq_header_data));
  EXPECT_FALSE(is_aac_audio(flv_aac_seq_header_data));

  EXPECT_FALSE(is_aac_sequence_header(flv_aac_data));
  EXPECT_TRUE(is_aac_audio(flv_aac_data));

  unsigned char* aac_seq_buffer= flv_aac_seq_header_data;
  std::size_t aac_seq_buffer_len = sizeof(aac_seq_buffer) / sizeof(unsigned char);
  uint32_t sample_rate;
  uint32_t channel_count;

  EXPECT_TRUE(get_aac_header(aac_seq_buffer, sample_rate, channel_count, aac_seq_buffer_len));

  unsigned char* aac_audio_buffer= flv_aac_data;
  std::size_t aac_audio_buffer_len = sizeof(aac_audio_buffer) / sizeof(unsigned char);
  buffer_t audioes(aac_audio_buffer_len);
  audio_frame_t audioframe;
  
  EXPECT_TRUE(get_audio(aac_audio_buffer, audioes, audioframe, aac_audio_buffer_len));
  EXPECT_TRUE(std::equal(audioes.ptr_, audioes.ptr_ + audioes.len_, flv_aac_audio_data));
}


TEST(FLV, get_avc_info)
{
  unsigned char flv_avc_seq_header_data[]= {
    0x17, 0x00, 0x00, 0x00, 0x00, 0x01, 0x64, 0x00,
    0x28, 0xff, 0xe1, 0x00, 0x29, 0x67, 0x64, 0x00,
    0x28, 0xac, 0xd9, 0x40, 0x78, 0x04, 0x4f, 0xde,
    0x03, 0x6a, 0x02, 0x02, 0x02, 0x80, 0x00, 0x01,
    0xf4, 0x80, 0x00, 0x75, 0x30, 0x71, 0x10, 0x00,
    0x09, 0x89, 0x68, 0x00, 0x05, 0x16, 0x17, 0xaf,
    0x7c, 0x07, 0x16, 0x86, 0x0a, 0x96, 0x01, 0x00,
    0x05, 0x68, 0xff, 0xb5, 0x25, 0x00
  };
  unsigned char flv_avc_sps_data[]= {
    0x67, 0x64, 0x00,
    0x28, 0xac, 0xd9, 0x40, 0x78, 0x04, 0x4f, 0xde,
    0x03, 0x6a, 0x02, 0x02, 0x02, 0x80, 0x00, 0x01,
    0xf4, 0x80, 0x00, 0x75, 0x30, 0x71, 0x10, 0x00,
    0x09, 0x89, 0x68, 0x00, 0x05, 0x16, 0x17, 0xaf,
    0x7c, 0x07, 0x16, 0x86, 0x0a, 0x96
  };
  unsigned char flv_avc_pps_data[]= {
    0x68, 0xff, 0xb5, 0x25, 0x00
  };

  unsigned char flv_avc_video_data_1[]= {
    0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x09, 0x10, 0x00, 0x00, 0x00, 0x29, 0x67,
    0x64, 0x00, 0x28, 0xac, 0xd9, 0x40, 0x78, 0x04,
    0x4f, 0xde, 0x03, 0x6a, 0x02, 0x02, 0x02, 0x80,
    0x00, 0x01, 0xf4, 0x80, 0x00, 0x75, 0x30, 0x71,
    0x10, 0x00, 0x09, 0x89, 0x68, 0x00, 0x05, 0x16,
    0x17, 0xaf, 0x7c, 0x07, 0x16, 0x86, 0x0a, 0x96,
    0x00, 0x00, 0x00, 0x04, 0x68, 0xff, 0xb5, 0x25
  };

  unsigned char flv_video_es_data[] = {
    0x00, 0x00, 0x00, 0x02, 
    0x09, 0x10, 
    0x00, 0x00, 0x00, 0x29, 
    0x67, 0x64, 0x00, 0x28, 0xac, 0xd9, 0x40, 0x78, 
    0x04, 0x4f, 0xde, 0x03, 0x6a, 0x02, 0x02, 0x02, 
    0x80, 0x00, 0x01, 0xf4, 0x80, 0x00, 0x75, 0x30, 
    0x71, 0x10, 0x00, 0x09, 0x89, 0x68, 0x00, 0x05, 
    0x16, 0x17, 0xaf, 0x7c, 0x07, 0x16, 0x86, 0x0a, 
    0x96, 0x00, 0x00, 0x00, 0x04, 0x68, 0xff, 0xb5, 
    0x25
  };

  unsigned char flv_avc_video_data_2[]= {
    0x27, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x09, 0x30, 0x00, 0x00, 0x00, 0x16, 0x06,
    0x01, 0x0e, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00,
    0x00, 0x03, 0x00, 0x00, 0x76, 0x48, 0x30, 0xa5,
    0x73, 0x40, 0x80, 0x00, 0x00
  };

  unsigned char flv_video_es_data2[] = {
    0x00, 0x00, 0x00, 0x02,
    0x09, 0x30, 
    0x00, 0x00, 0x00, 0x16, 
    0x06, 0x01, 0x0e, 0x00, 0x00, 0x03, 0x00, 0x04,
    0x00, 0x00, 0x03, 0x00, 0x00, 0x76, 0x48, 0x30, 
    0xa5, 0x73, 0x40, 0x80, 0x00, 0x00
  };

  EXPECT_FALSE(is_avc_video(flv_avc_seq_header_data));
  EXPECT_TRUE(is_avc_video(flv_avc_video_data_1));
  EXPECT_TRUE(is_avc_video(flv_avc_video_data_2));

  EXPECT_TRUE(is_avc_sequence_header(flv_avc_seq_header_data));
  EXPECT_FALSE(is_avc_sequence_header(flv_avc_video_data_1));
  EXPECT_FALSE(is_avc_sequence_header(flv_avc_video_data_2));

  unsigned char* buffer = flv_avc_seq_header_data;
  std::size_t buffer_len = sizeof(flv_avc_seq_header_data) / sizeof(unsigned char);
  buffer_t sps(65535);
  buffer_t pps(65535);
  uint8_t nal_startcode_len;
  EXPECT_TRUE(get_first_sps_and_first_pps_from_avc_sequence_header(buffer, sps, pps, nal_startcode_len, buffer_len));
  
  EXPECT_TRUE(std::equal(sps.ptr_, sps.ptr_ + sps.len_, flv_avc_sps_data));
  EXPECT_TRUE(std::equal(pps.ptr_, pps.ptr_ + pps.len_, flv_avc_pps_data));

  std::string sps_encoded_string;
  sps_encoded_string = encoding_base_64(sps);

  std::string pps_encoded_string;
  pps_encoded_string = encoding_base_64(pps);

  unsigned char* flv_video_buffer = flv_avc_video_data_1;
  std::size_t flv_video_buffer_len = sizeof(flv_avc_video_data_1) / sizeof(unsigned char);
  buffer_t videoes(flv_video_buffer_len);
  video_frame_t videoframe;
  
  EXPECT_TRUE(get_video(flv_video_buffer,videoes, videoframe, flv_video_buffer_len));
  EXPECT_TRUE(std::equal(videoes.ptr_, videoes.ptr_ + videoes.len_, flv_video_es_data));
  EXPECT_EQ(1, videoframe.keyframe_);

  unsigned char* flv_video_buffer2 = flv_avc_video_data_2;
  std::size_t flv_video_buffer_len2 = sizeof(flv_avc_video_data_2) / sizeof(unsigned char);
  buffer_t videoes2(flv_video_buffer_len2);
  video_frame_t videoframe2;
  
  EXPECT_TRUE(get_video(flv_video_buffer2 ,videoes2 , videoframe2, flv_video_buffer_len2));
  EXPECT_TRUE(std::equal(videoes2.ptr_, videoes2.ptr_ + videoes2.len_, flv_video_es_data2));
  EXPECT_EQ(2, videoframe2.keyframe_);
}


TEST(FLV, write_n)
{
  unsigned char data[]= {
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFF
  };
  unsigned char double_data[]= {
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFF,
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFF
  };
  unsigned char three_data[]= {
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFF,
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFF,
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFF
  };

  size_t data_len = sizeof(data) / sizeof (unsigned char);

  buffer_t buffer(12);
  read_nbyte(data, data_len, buffer);

  buffer_t dst_buffer(36);
  
  unsigned char* writte_ptr = dst_buffer.ptr_;
  std::size_t written{0};
  write_nbyte(buffer, writte_ptr, written); 
  dst_buffer.len_ = written;

  EXPECT_TRUE(10 == written);
  EXPECT_TRUE(10 == dst_buffer.len_);

  EXPECT_TRUE(std::equal(dst_buffer.ptr_, dst_buffer.ptr_ + dst_buffer.len_, data));

  write_nbyte(buffer, writte_ptr, written); 
  dst_buffer.len_ = written;

  EXPECT_TRUE(20 == written);
  EXPECT_TRUE(20 ==  dst_buffer.len_);

  EXPECT_TRUE(std::equal(dst_buffer.ptr_, dst_buffer.ptr_ + dst_buffer.len_, double_data));

  unsigned char* data_ptr = data;
  write_nbyte(data_ptr, data_len, writte_ptr, data_len, written); 
  dst_buffer.len_ = written;

  EXPECT_TRUE(0==data_len);
  EXPECT_TRUE(30== written);
  EXPECT_TRUE(30 == dst_buffer.len_);

  EXPECT_TRUE(std::equal(dst_buffer.ptr_, dst_buffer.ptr_ + dst_buffer.len_, three_data));  
}

// 4 byte 까지만 성공함
//4 byte 이상에 대해서는 실패함
TEST(FLV, read_nbyte_big_endian_to_little_endian)
{
  unsigned char data4[]= {0xfe, 0x01, 0x12, 0x02};

  buffer_t buffer(4);
  read_nbyte(data4, 4, buffer);

  EXPECT_TRUE(4 == buffer.len_);

  uint32_t value32;
  get_4byte(data4, value32);
  EXPECT_TRUE(4261483010 == value32);

  read_nbyte(buffer, value32);
  EXPECT_TRUE(4261483010 == value32);
}

TEST(FLV, read_4byte_big_endian_to_little_endian)
{
  unsigned char data1[]= {0x00, 0x00, 0x00, 0x02};
  unsigned char data2[]= {0x00, 0x00, 0x12, 0x02};
  unsigned char data3[]= {0x00, 0x01, 0x12, 0x02};
  unsigned char data4[]= {0xfe, 0x01, 0x12, 0x02};

  buffer_t buffer(4);
  read_nbyte(data1, 4, buffer);

  EXPECT_TRUE(4 == buffer.len_);

  uint32_t value32;
  read_nbyte(buffer, value32);
  EXPECT_TRUE(2 == value32);

  get_4byte(data2, value32);
  EXPECT_TRUE(4610 == value32);

  get_4byte(data3, value32);
  EXPECT_TRUE(70146== value32);

  get_4byte(data4, value32);
  EXPECT_TRUE(4261483010== value32);
}


TEST(FLV, get_last_and_put_last)
{
  unsigned char len2[]= {0x00, 0x00, 0x00, 0x02};
  unsigned char len41[]= {0x00, 0x00, 0x00, 0x29};

  unsigned char* ptr2 = len2;
  size_t len_2 = 4;
  buffer_t buf_len2(len_2);
  read_nbyte(ptr2, len_2, buf_len2, len_2); 

  EXPECT_EQ(2, get_last_byte(buf_len2));

  put_last_byte(buf_len2, 0x01);
  EXPECT_EQ(0x01, get_last_byte(buf_len2));

  unsigned char* ptr41 = len41;
  size_t len_41 = 4;
  buffer_t buf_len41(len_41);
  read_nbyte(ptr41, len_41, buf_len41, len_41); 

  EXPECT_EQ(41, get_last_byte(buf_len41));

  put_last_byte(buf_len41, 0x01);
  EXPECT_EQ(0x01, get_last_byte(buf_len41));

  unsigned char len41_6[]= {0x00, 0x00, 0x00, 0x00, 0x00, 0x29};
  unsigned char* ptr41_6 = len41_6;
  size_t len_41_6 = 6;
  buffer_t buf_len41_6(len_41_6);
  read_nbyte(ptr41_6, len_41_6, buf_len41_6, len_41_6); 

  EXPECT_EQ(41, get_last_byte(buf_len41_6));
  
  put_last_byte(buf_len41_6, 0x01);
  EXPECT_EQ(0x01, get_last_byte(buf_len41_6));
  
}

TEST(FLV, read_n_byte_again_and_again)
{
  unsigned char data[]= {
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFF
  };
  unsigned char false_data[]= {
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFE
  };
  size_t data_len = sizeof(data) / sizeof (unsigned char);

  buffer_t buffer(12);
  EXPECT_TRUE(12 == buffer.max_len_);  
  EXPECT_TRUE(0 == buffer.len_);  

  read_nbyte(data, data_len, buffer);

  EXPECT_TRUE(10 == buffer.len_);
  EXPECT_TRUE(10 == data_len);

  EXPECT_TRUE(std::equal(buffer.ptr_, buffer.ptr_ + buffer.len_, data));
  EXPECT_FALSE(std::equal(buffer.ptr_, buffer.ptr_ + buffer.len_, false_data));

  // read again
  read_nbyte(data, data_len, buffer);
  EXPECT_TRUE(10== buffer.len_);
  EXPECT_TRUE(10 ==data_len);
  EXPECT_TRUE(std::equal(buffer.ptr_, buffer.ptr_ + buffer.len_, data));
  EXPECT_FALSE(std::equal(buffer.ptr_, buffer.ptr_ + buffer.len_, false_data));

  buffer_t dst_buffer(12);
  read_nbyte(buffer, dst_buffer);

  EXPECT_TRUE(12== dst_buffer.max_len_);
  EXPECT_TRUE(10 == dst_buffer.len_);

  EXPECT_TRUE(std::equal(buffer.ptr_, buffer.ptr_ + buffer.len_, dst_buffer.ptr_));

  // read again
  read_nbyte(buffer, dst_buffer);

  EXPECT_TRUE(12== dst_buffer.max_len_);
  EXPECT_TRUE(10== dst_buffer.len_);

  EXPECT_TRUE(std::equal(buffer.ptr_, buffer.ptr_ + buffer.len_, dst_buffer.ptr_));
}

TEST(FLV, read_4byte)
{
  unsigned char data1[]= {0x00, 0x00, 0x00, 0x02};
  unsigned char data2[]= {0x00, 0x00, 0x12, 0x02};
  unsigned char data3[]= {0x00, 0x01, 0x12, 0x02};
  unsigned char data4[]= {0xfe, 0x01, 0x12, 0x02};

  unsigned char* buffer = data1;
  std::size_t len = 4;

  uint32_t value32;
  read_4byte(buffer, value32, len);
  EXPECT_TRUE(2== value32);
  EXPECT_TRUE(0== len);

  unsigned char* buffer2 = data2;
  std::size_t len2 = 4;
  read_4byte(buffer2, value32, len2);
  EXPECT_TRUE(4610== value32);
  EXPECT_TRUE(0== len2);

  unsigned char* buffer3 = data3;
  std::size_t len3 = 4;
  read_4byte(buffer3, value32, len3);
  EXPECT_TRUE(70146== value32);
  EXPECT_TRUE(0== len3);

  unsigned char* buffer4 = data4;
  std::size_t len4 = 4;
  read_4byte(buffer4, value32, len4);
  EXPECT_TRUE(4261483010== value32);
  EXPECT_TRUE(0==len4);
}

TEST(FLV, read3_bytes_read3_buffer)
{
  unsigned char buffer[]= {0x00, 0x01, 0x09};
  
  unsigned char* ptr_1 = buffer;
  size_t len_1 = 3;

  unsigned char* ptr_2 = buffer;
  size_t len_2 = 3;
  
  uint32_t cts_1;
  read_3byte(ptr_1, cts_1, len_1);

  EXPECT_TRUE(0== len_1);
  EXPECT_TRUE(265== cts_1);

  buffer_t composition_timestamp_offset(3);
  read_nbyte(ptr_2, 3, composition_timestamp_offset, len_2); 
  EXPECT_TRUE(0== len_2);
  EXPECT_TRUE(3== composition_timestamp_offset.len_);

  uint32_t cts_2 = 
    (composition_timestamp_offset.ptr_[0] << 16) |
    (composition_timestamp_offset.ptr_[1] << 8) |
    composition_timestamp_offset.ptr_[2];

  EXPECT_TRUE(265== cts_2);
  EXPECT_TRUE(cts_1== cts_2);
}

TEST(FLV, read_bytes2)
{
  unsigned char data[]= {
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFF
  };
  unsigned char false_data[]= {
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFE
  };

  unsigned char* buffer = data;
  size_t data_len = sizeof(data) / sizeof (unsigned char);
  EXPECT_TRUE(10== data_len);

  buffer_t tbuffer(12);
  EXPECT_TRUE(12== tbuffer.max_len_);  
  EXPECT_TRUE(0== tbuffer.len_);  

  read_nbyte(buffer, data_len, tbuffer, data_len);

  EXPECT_TRUE(10== tbuffer.len_);
  EXPECT_TRUE(0== data_len);

  EXPECT_TRUE(std::equal(tbuffer.ptr_, tbuffer.ptr_ + tbuffer.len_, data));
  EXPECT_FALSE(std::equal(tbuffer.ptr_, tbuffer.ptr_ + tbuffer.len_, false_data));
}


TEST(FLV, read_bytes)
{
  unsigned char data[]= {
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFF
  };
  unsigned char false_data[]= {
    0x64, 0x00, 0x28, 0xAC, 0xD9, 0x40, 0x78, 0x04,
    0x02, 0xFE
  };

  unsigned char* buffer = data;
  size_t data_len = sizeof(data) / sizeof (unsigned char);
  EXPECT_TRUE(10== data_len);

  buffer_t tbuffer(12);
  EXPECT_TRUE(12== tbuffer.max_len_);  
  EXPECT_TRUE(0== tbuffer.len_);  

  tbuffer.len_ = data_len;  
  read_nbyte(buffer, data_len, tbuffer.ptr_, data_len);

  EXPECT_TRUE(10== tbuffer.len_);
  EXPECT_TRUE(0== data_len);
  EXPECT_TRUE(std::equal(tbuffer.ptr_, tbuffer.ptr_ + tbuffer.len_, data));
  EXPECT_FALSE(std::equal(tbuffer.ptr_, tbuffer.ptr_ + tbuffer.len_, false_data));
}

TEST(FLV, read2_buffer)
{
  unsigned char buffer[]= {0x01, 0x09};
  unsigned char* ptr = buffer;
  size_t len = 2;
  uint16_t cts;
  read_2byte(ptr, cts, len);

  EXPECT_TRUE(265== cts);  
  string str = to_hex(cts);
  EXPECT_TRUE("01 09"== str);  
}

TEST(FLV, read3_buffer)
{
  unsigned char buffer[]= {0x00, 0x01, 0x09};
  unsigned char* ptr = buffer;
  size_t len = 3;
  uint32_t cts;
  read_3byte(ptr, cts, len);

  EXPECT_TRUE(265== cts);  
  string str = to_hex(cts);
  EXPECT_TRUE("00 00 01 09"== str);  
}

TEST(FLV, to_hex)
{
  unsigned char dat[] =  {0x00, 0x00, 0x01};
  unsigned char dat2[] =  {0x01, 0x00, 0x00};

  string str = to_hex(dat, 3);
  EXPECT_TRUE("00 00 01"== str);
  
  str = to_hex(dat2, 3);
  EXPECT_TRUE("01 00 00"== str);

  uint16_t v16 = 0xe1;
  str = to_hex(v16);
  EXPECT_TRUE("00 E1"== str);

  v16 = 0x12e1;
  str = to_hex(v16);
  EXPECT_TRUE("12 E1"== str);
  
  uint32_t v32 = 0xe1;
  str = to_hex(v32);
  EXPECT_TRUE("00 00 00 E1"== str);

  v32 = 0x100102e1;
  str = to_hex(v32);
  EXPECT_TRUE("10 01 02 E1"== str);
}

