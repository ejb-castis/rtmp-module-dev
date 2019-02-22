#include <gtest/gtest.h>
#include <array>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

TEST(TEST_MEMCOPY, memcopy_pointer_to_variable)
{
  // little endian 1
  unsigned char data[]= {
    0x01, 0x00, 0x00, 0x00 
  };
  uint32_t number = 0 ;
  memcpy(&number, data, 4);
  EXPECT_EQ(number, 0x00000001U);
}

TEST(TEST_MEMCOPY, memcopy_variable_to_pointer)
{
  unsigned char data[4];
  uint32_t number = 0x00000001U;
  memcpy(data, &number, 4);
  
  // little endian 1
  unsigned char value[]= {
    0x01, 0x00, 0x00, 0x00 
  };
  EXPECT_TRUE(std::equal(&data[0], &data[4], &value[0]));
}

TEST(TEST_MEMCOPY, memcopy_do_128_little_endian_to_big_endian)
{
  uint32_t number = 128;
  unsigned char data[4];
  memcpy(data, &number, 4);
  
  // little endian 128
  unsigned char value[]= {
    0x80, 0x00, 0x00, 0x00
  };

  EXPECT_TRUE(std::equal(&data[0], &data[4], &value[0]));
}


