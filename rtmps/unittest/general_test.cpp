#include <gtest/gtest.h>
#include <array>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

TEST(TEST_MEMCOPY, memcopy_do__big_endian_to_little_endian)
{
  unsigned char data[]= {
    0x01, 0x00, 0x00, 0x00 
  };
  uint32_t number = 0 ;
  memcpy(&number, data, 4);
  EXPECT_EQ(number, 0x00000001U);

  unsigned char data2[]= {
    0x00, 0x00, 0x00, 0x01 
  };
  uint32_t number2 = 0 ;
  memcpy(&number2, data2, 4);
  EXPECT_EQ(number2, 0x01000000U);
}

TEST(TEST_MEMCOPY, memcopy_do_little_endian_to_big_endian)
{
  unsigned char data[4];
  uint32_t number = 0x00000001U;
  memcpy(data, &number, 4);
  
  
  unsigned char value[]= {
    0x01, 0x00, 0x00, 0x00 
  };
  EXPECT_TRUE(std::equal(&data[0], &data[4], &value[0]));
}
