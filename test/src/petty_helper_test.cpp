//
// Copyright 2016 imai hiroyuki.
// All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
#include <memory>
#include <string>

#include "gtest/gtest.h"
#include "pettyutil/petty_helper.h"

namespace {

/// pettyutil::NullOption not null test.
///
TEST(NullOptionTest, NotNull) {
  const char* test1 = pettyutil::NullOption("not null", "is null");
  ASSERT_TRUE(test1 != nullptr);
  EXPECT_STREQ("not null", test1);

  int test2 = pettyutil::NullOption(std::unique_ptr<int>(new int(1)).get(), 0);
  EXPECT_EQ(1, test2);

  auto test3 = std::unique_ptr<std::string>(new std::string("test3"));
  auto test4 = pettyutil::NullOption(test3.get(), std::string("option"));
  EXPECT_STREQ("test3", test4.c_str());
  ASSERT_TRUE(test3.get() != nullptr);
  EXPECT_STREQ("test3", test3->c_str());
}

/// pettyutil::NullOption is null test.
///
TEST(NullOptionTest, IsNull) {
  const char* test1 = pettyutil::NullOption<const char*>(nullptr, "is null");
  ASSERT_TRUE(test1 != nullptr);
  EXPECT_STREQ("is null", test1);

  int test2 = pettyutil::NullOption(std::unique_ptr<int>().get(), 0);
  EXPECT_EQ(0, test2);

  auto test3 = pettyutil::NullOption(std::unique_ptr<std::string>().get(),
                                     std::string("option"));
  EXPECT_STREQ("option", test3.c_str());
}

/// pettyutil::CountOf test.
///
TEST(CountOfTest, Count) {
  char test_char1[1];
  char test_char100[100];
  int test_int1[1];
  int test_int100[100];

  EXPECT_EQ(1, pettyutil::CountOf(test_char1));
  EXPECT_EQ(100, pettyutil::CountOf(test_char100));
  EXPECT_EQ(1, pettyutil::CountOf(test_int1));
  EXPECT_EQ(100, pettyutil::CountOf(test_int100));
}

}  // namespace
