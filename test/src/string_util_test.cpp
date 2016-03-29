//
// Copyright 2016 imai hiroyuki.
// All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
#include <stdarg.h>
#include <cstdint>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "pettyutil/string_util.h"

namespace {

/// pettyutil::StringFormat format test.
///
TEST(StringFormatTest, Format) {
  auto test = pettyutil::StringFormat("test%d", 1);
  EXPECT_STREQ("test1", test.c_str());
}

std::string SubFormat(const char* format, ...) {
  va_list args;
  va_start(args, format);
  auto value = pettyutil::StringFormatV(format, args);
  va_end(args);

  return std::move(value);
}

/// pettyutil::StringFormatV format test.
///
TEST(StringFormatVTest, Format) {
  auto test = SubFormat("test%d", 2);
  EXPECT_STREQ("test2", test.c_str());
}

/// pettyutil::TryStringToInteger octal test.
///
TEST(TryStringToIntegerTest, Octal) {
  int test1_value = 0;
  bool test1_result = pettyutil::TryStringToInteger("10", 8, &test1_value);
  EXPECT_TRUE(test1_result);
  EXPECT_EQ(8, test1_value);

  int test2_value = 0;
  bool test2_result = pettyutil::TryStringToInteger("+12", 8, &test2_value);
  EXPECT_TRUE(test2_result);
  EXPECT_EQ(10, test2_value);

  int test3_value = 0;
  bool test3_result = pettyutil::TryStringToInteger("-20", 8, &test3_value);
  EXPECT_TRUE(test3_result);
  EXPECT_EQ(-16, test3_value);

  int test4_value = 0;
  bool test4_result = pettyutil::TryStringToInteger("00", 8, &test4_value);
  EXPECT_TRUE(test4_result);
  EXPECT_EQ(0, test4_value);

  int test5_value = 0;
  bool test5_result = pettyutil::TryStringToInteger("8", 8, &test5_value);
  EXPECT_FALSE(test5_result);

  unsigned int test6_value = 0;
  bool test6_result = pettyutil::TryStringToInteger("-7", 8, &test6_value);
  EXPECT_FALSE(test6_result);
}

/// pettyutil::TryStringToInteger decimal test.
///
TEST(TryStringToIntegerTest, Decimal) {
  int test1_value = 0;
  bool test1_result = pettyutil::TryStringToInteger("8", 10, &test1_value);
  EXPECT_TRUE(test1_result);
  EXPECT_EQ(8, test1_value);

  int test2_value = 0;
  bool test2_result = pettyutil::TryStringToInteger("+10", 10, &test2_value);
  EXPECT_TRUE(test2_result);
  EXPECT_EQ(10, test2_value);

  int test3_value = 0;
  bool test3_result = pettyutil::TryStringToInteger("-16", 10, &test3_value);
  EXPECT_TRUE(test3_result);
  EXPECT_EQ(-16, test3_value);

  int test4_value = 0;
  bool test4_result = pettyutil::TryStringToInteger("00", 10, &test4_value);
  EXPECT_TRUE(test4_result);
  EXPECT_EQ(0, test4_value);

  int test5_value = 0;
  bool test5_result = pettyutil::TryStringToInteger("A", 10, &test5_value);
  EXPECT_FALSE(test5_result);

  unsigned int test6_value = 0;
  bool test6_result = pettyutil::TryStringToInteger("-9", 10, &test6_value);
  EXPECT_FALSE(test6_result);
}

/// pettyutil::TryStringToInteger hex test.
///
TEST(TryStringToIntegerTest, Hex) {
  int test1_value = 0;
  bool test1_result = pettyutil::TryStringToInteger("8", 16, &test1_value);
  EXPECT_TRUE(test1_result);
  EXPECT_EQ(8, test1_value);

  int test2_value = 0;
  bool test2_result = pettyutil::TryStringToInteger("+A", 16, &test2_value);
  EXPECT_TRUE(test2_result);
  EXPECT_EQ(10, test2_value);

  int test3_value = 0;
  bool test3_result = pettyutil::TryStringToInteger("f", 16, &test3_value);
  EXPECT_TRUE(test3_result);
  EXPECT_EQ(15, test3_value);

  int test4_value = 0;
  bool test4_result = pettyutil::TryStringToInteger("-10", 16, &test4_value);
  EXPECT_TRUE(test4_result);
  EXPECT_EQ(-16, test4_value);

  int test5_value = 0;
  bool test5_result = pettyutil::TryStringToInteger("00", 16, &test5_value);
  EXPECT_TRUE(test5_result);
  EXPECT_EQ(0, test5_value);

  int test6_value = 0;
  bool test6_result = pettyutil::TryStringToInteger("G", 16, &test6_value);
  EXPECT_FALSE(test6_result);

  unsigned int test7_value = 0;
  bool test7_result = pettyutil::TryStringToInteger("-F", 16, &test7_value);
  EXPECT_FALSE(test7_result);
}

/// pettyutil::TryStringToInteger auto test.
///
TEST(TryStringToIntegerTest, Auto) {
  int test1_value = 0;
  bool test1_result = pettyutil::TryStringToInteger("010", 0, &test1_value);
  EXPECT_TRUE(test1_result);
  EXPECT_EQ(8, test1_value);

  int test2_value = 0;
  bool test2_result = pettyutil::TryStringToInteger("10", 0, &test2_value);
  EXPECT_TRUE(test2_result);
  EXPECT_EQ(10, test2_value);

  int test3_value = 0;
  bool test3_result = pettyutil::TryStringToInteger("0x10", 0, &test3_value);
  EXPECT_TRUE(test3_result);
  EXPECT_EQ(16, test3_value);

  int test4_value = 0;
  bool test4_result = pettyutil::TryStringToInteger("0X0F", 0, &test4_value);
  EXPECT_TRUE(test4_result);
  EXPECT_EQ(15, test4_value);
}

/// pettyutil::StringToInteger octal test.
///
TEST(StringToIntegerTest, Octal) {
  int test1_value = pettyutil::StringToInteger<int>("10", 8);
  EXPECT_EQ(8, test1_value);

  int test2_value = pettyutil::StringToInteger<int>("+12", 8);
  EXPECT_EQ(10, test2_value);

  int test3_value = pettyutil::StringToInteger<int>("-20", 8);
  EXPECT_EQ(-16, test3_value);

  int test4_value = pettyutil::StringToInteger<int>("00", 8);
  EXPECT_EQ(0, test4_value);

  int test5_value = pettyutil::StringToInteger<int>("9", 8);
  EXPECT_EQ(0, test5_value);
}

/// pettyutil::StringToInteger decimal test.
///
TEST(StringToIntegerTest, Decimal) {
  int test1_value = pettyutil::StringToInteger<int>("8", 10);
  EXPECT_EQ(8, test1_value);

  int test2_value = pettyutil::StringToInteger<int>("+10", 10);
  EXPECT_EQ(10, test2_value);

  int test3_value = pettyutil::StringToInteger<int>("-16", 10);
  EXPECT_EQ(-16, test3_value);

  int test4_value = pettyutil::StringToInteger<int>("00", 10);
  EXPECT_EQ(0, test4_value);

  int test5_value = pettyutil::StringToInteger<int>("A", 10);
  EXPECT_EQ(0, test5_value);
}

/// pettyutil::StringToInteger hex test.
///
TEST(StringToIntegerTest, Hex) {
  int test1_value = pettyutil::StringToInteger<int>("8", 16);
  EXPECT_EQ(8, test1_value);

  int test2_value = pettyutil::StringToInteger<int>("+A", 16);
  EXPECT_EQ(10, test2_value);

  int test3_value = pettyutil::StringToInteger<int>("f", 16);
  EXPECT_EQ(15, test3_value);

  int test4_value = pettyutil::StringToInteger<int>("-10", 16);
  EXPECT_EQ(-16, test4_value);

  int test5_value = pettyutil::StringToInteger<int>("00", 16);
  EXPECT_EQ(0, test5_value);

  int test6_value = pettyutil::StringToInteger<int>("G", 16);
  EXPECT_EQ(0, test6_value);
}

/// pettyutil::StringToInteger auto test.
///
TEST(StringToIntegerTest, Auto) {
  int test1_value = pettyutil::StringToInteger<int>("010", 0);
  EXPECT_EQ(8, test1_value);

  int test2_value = pettyutil::StringToInteger<int>("10", 0);
  EXPECT_EQ(10, test2_value);

  int test3_value = pettyutil::StringToInteger<int>("0x10", 0);
  EXPECT_EQ(16, test3_value);

  int test4_value = pettyutil::StringToInteger<int>("0X0F", 0);
  EXPECT_EQ(15, test4_value);
}

/// pettyutil::StringStartsWith true test.
///
TEST(StringStartsWithTest, True) {
  bool test1 = pettyutil::StringStartsWith("test1", "test");
  EXPECT_TRUE(test1);

  bool test2 = pettyutil::StringStartsWith("test2", "test2");
  EXPECT_TRUE(test2);
}

/// pettyutil::StringStartsWith false test.
///
TEST(StringStartsWithTest, False) {
  bool test1 = pettyutil::StringStartsWith("1test", "test");
  EXPECT_FALSE(test1);

  bool test2 = pettyutil::StringStartsWith("test2", "TEST2");
  EXPECT_FALSE(test2);

  bool test3 = pettyutil::StringStartsWith("test", "test3");
  EXPECT_FALSE(test3);
}

/// pettyutil::StringEndsWith true test.
///
TEST(StringEndsWithTest, True) {
  bool test1 = pettyutil::StringEndsWith("1test", "test");
  EXPECT_TRUE(test1);

  bool test2 = pettyutil::StringEndsWith("2test", "2test");
  EXPECT_TRUE(test2);
}

/// pettyutil::StringEndsWith false test.
///
TEST(StringEndsWithTest, False) {
  bool test1 = pettyutil::StringEndsWith("test1", "test");
  EXPECT_FALSE(test1);

  bool test2 = pettyutil::StringEndsWith("2test", "2TEST");
  EXPECT_FALSE(test2);

  bool test3 = pettyutil::StringEndsWith("test", "3test");
  EXPECT_FALSE(test3);
}

/// pettyutil::StringReplaceAll replace test.
///
TEST(StringReplaceAllTest, Replace) {
  auto test1 = pettyutil::StringReplaceAll("aaaa bbbb cccc", "bbbb", "dddd");
  EXPECT_STREQ("aaaa dddd cccc", test1.c_str());

  auto test2 = pettyutil::StringReplaceAll("aaaa bbbb aaaa", "aaaa", "dddd");
  EXPECT_STREQ("dddd bbbb dddd", test2.c_str());

  auto test3 = pettyutil::StringReplaceAll("aaaa bbbb cccc", "dddd", "eeee");
  EXPECT_STREQ("aaaa bbbb cccc", test3.c_str());

  auto test4 = pettyutil::StringReplaceAll("a aa aaa aaaa", "aa", "a");
  EXPECT_STREQ("a a aa aa", test4.c_str());
}

/// pettyutil::StringSplit replace test.
///
TEST(StringSplitTest, Split) {
  auto test1 = pettyutil::StringSplit("test1:test2:test3",
                                      pettyutil::IsAnyOf(":"));
  ASSERT_EQ(3, test1.size());
  EXPECT_STREQ("test1", test1.at(0).c_str());
  EXPECT_STREQ("test2", test1.at(1).c_str());
  EXPECT_STREQ("test3", test1.at(2).c_str());

  auto test2 = pettyutil::StringSplit("test1,test2 test3",
                                      pettyutil::IsAnyOf(", "));
  ASSERT_EQ(3, test2.size());
  EXPECT_STREQ("test1", test2.at(0).c_str());
  EXPECT_STREQ("test2", test2.at(1).c_str());
  EXPECT_STREQ("test3", test2.at(2).c_str());

  auto test3 = pettyutil::StringSplit("test1test2test3",
                                      pettyutil::IsAnyOf(":"));
  ASSERT_EQ(1, test3.size());
  EXPECT_STREQ("test1test2test3", test3.at(0).c_str());

  auto test4 = pettyutil::StringSplit(":test1::test2::test3:",
                                      pettyutil::IsAnyOf(":"));
  ASSERT_EQ(7, test4.size());
  EXPECT_TRUE(test4.at(0).empty());
  EXPECT_STREQ("test1", test4.at(1).c_str());
  EXPECT_TRUE(test4.at(2).empty());
  EXPECT_STREQ("test2", test4.at(3).c_str());
  EXPECT_TRUE(test4.at(4).empty());
  EXPECT_STREQ("test3", test4.at(5).c_str());
  EXPECT_TRUE(test4.at(6).empty());
}

/// pettyutil::ArrayToString test fixture.
///
class ArrayToStringTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    int_array_.push_back(0x00);
    int_array_.push_back(0x7f);
    int_array_.push_back(0x80);
    int_array_.push_back(0xff);

    ascii_array_.push_back(0x41);
    ascii_array_.push_back(0x42);
    ascii_array_.push_back(0x43);
    ascii_array_.push_back(0x44);
  }

  std::vector<int> int_array_;
  std::vector<int> ascii_array_;
};

/// pettyutil::ArrayToString to hex byte test.
///
TEST_F(ArrayToStringTest, ToHexByte) {
  auto test1 = pettyutil::ArrayToString(int_array_.begin(),
                                        int_array_.end(),
                                        pettyutil::ToHexByte(":"));
  EXPECT_STREQ("00:7F:80:FF", test1.c_str());

  auto test2 = pettyutil::ArrayToString(int_array_.begin(),
                                        int_array_.end(),
                                        pettyutil::ToHexByte(" ", false));
  EXPECT_STREQ("00 7f 80 ff", test2.c_str());

  auto test3 = pettyutil::ArrayToString(ascii_array_.begin(),
                                        ascii_array_.end(),
                                        pettyutil::ToHexByte(""));
  EXPECT_STREQ("41424344", test3.c_str());
}

/// pettyutil::ArrayToString to multi byte test.
///
TEST_F(ArrayToStringTest, ToMultiByte) {
  auto test = pettyutil::ArrayToString(ascii_array_.begin(),
                                       ascii_array_.end(),
                                       pettyutil::ToMultiByte());
  EXPECT_STREQ("ABCD", test.c_str());
}

}  // namespace
