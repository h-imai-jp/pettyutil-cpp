//
// Copyright 2016 imai hiroyuki.
// All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
#include <cstdint>
#include <string>

#include "gtest/gtest.h"
#include "pettyutil/unsorted_map.h"

namespace {

/// Test unsorted map.
///
typedef pettyutil::UnsortedMap<int, std::string> TestMap;

/// pettyutil::UnsortedMap test fixture.
///
class UnsortedMapTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    notempty_.Insert(TestMap::ValueType(3, "test3"));
    notempty_.Insert(TestMap::ValueType(1, "test1"));
    notempty_.Insert(TestMap::ValueType(2, "test2"));
  }

  TestMap notempty_;
  TestMap empty_;
};

/// pettyutil::UnsortedMap empty test.
///
TEST_F(UnsortedMapTest, Empty) {
  EXPECT_TRUE(empty_.Empty());
  EXPECT_EQ(0, empty_.Size());
}

/// pettyutil::UnsortedMap const empty test.
///
TEST_F(UnsortedMapTest, ConstEmpty) {
  const TestMap& empty = empty_;

  EXPECT_TRUE(empty.Empty());
  EXPECT_EQ(0, empty.Size());
}

/// pettyutil::UnsortedMap not empty test.
///
TEST_F(UnsortedMapTest, NotEmpty) {
  EXPECT_FALSE(notempty_.Empty());
  ASSERT_EQ(3, notempty_.Size());

  int index = 0;
  for (TestMap::Iterator it = notempty_.Begin();
      it != notempty_.End(); ++it, index++) {
    switch (index) {
      case 0:
        EXPECT_EQ(3, it->first);
        EXPECT_STREQ("test3", it->second.c_str());
        break;
      case 1:
        EXPECT_EQ(1, it->first);
        EXPECT_STREQ("test1", it->second.c_str());
        break;
      case 2:
        EXPECT_EQ(2, it->first);
        EXPECT_STREQ("test2", it->second.c_str());
        break;
      default:
        FAIL();
        break;
    }
  }
}

/// pettyutil::UnsortedMap const not empty test.
///
TEST_F(UnsortedMapTest, ConstNotEmpty) {
  const TestMap& notempty = notempty_;

  EXPECT_FALSE(notempty.Empty());
  ASSERT_EQ(3, notempty.Size());

  int index = 0;
  for (TestMap::ConstIterator it = notempty.Begin();
      it != notempty.End(); ++it, index++) {
    switch (index) {
      case 0:
        EXPECT_EQ(3, it->first);
        EXPECT_STREQ("test3", it->second.c_str());
        break;
      case 1:
        EXPECT_EQ(1, it->first);
        EXPECT_STREQ("test1", it->second.c_str());
        break;
      case 2:
        EXPECT_EQ(2, it->first);
        EXPECT_STREQ("test2", it->second.c_str());
        break;
      default:
        FAIL();
        break;
    }
  }
}

/// pettyutil::UnsortedMap insert test.
///
TEST_F(UnsortedMapTest, Insert) {
  TestMap test;

  std::pair<TestMap::Iterator, bool> result1 = test.Insert(
      TestMap::ValueType(1, "test"));
  EXPECT_EQ(test.Begin(), result1.first);
  EXPECT_TRUE(result1.second);
  EXPECT_EQ(1, test.Size());

  std::pair<TestMap::Iterator, bool> result2 = test.Insert(
      TestMap::ValueType(1, "test"));
  EXPECT_EQ(test.Begin(), result2.first);
  EXPECT_FALSE(result2.second);
  EXPECT_EQ(1, test.Size());

  std::pair<TestMap::Iterator, bool> result3 = test.Insert(
      TestMap::ValueType(2, "test"));
  EXPECT_EQ(test.Begin() + 1, result3.first);
  EXPECT_TRUE(result3.second);
  EXPECT_EQ(2, test.Size());
}

/// pettyutil::UnsortedMap position insert test.
///
TEST_F(UnsortedMapTest, PositionInsert) {
  TestMap test;

  TestMap::Iterator result1 = test.Insert(
      test.End(), TestMap::ValueType(1, "test"));
  EXPECT_EQ(test.Begin(), result1);
  EXPECT_EQ(1, test.Size());

  TestMap::Iterator result2 = test.Insert(
      test.End(), TestMap::ValueType(1, "test"));
  EXPECT_EQ(test.Begin(), result2);
  EXPECT_EQ(1, test.Size());

  TestMap::Iterator result3 = test.Insert(
      test.Begin(), TestMap::ValueType(2, "test"));
  EXPECT_EQ(test.Begin(), result3);
  EXPECT_EQ(2, test.Size());
}

/// pettyutil::UnsortedMap element test.
///
TEST_F(UnsortedMapTest, Element) {
  TestMap test;

  test[1] = "test1";
  EXPECT_EQ(1, test.Size());
  EXPECT_STREQ("test1", test[1].c_str());

  test[1] = "test2";
  EXPECT_EQ(1, test.Size());
  EXPECT_STREQ("test2", test[1].c_str());

  test[3] = "test3";
  EXPECT_EQ(2, test.Size());
  EXPECT_STREQ("test3", test[3].c_str());

  EXPECT_TRUE(test[4].empty());
  EXPECT_EQ(3, test.Size());
}

/// pettyutil::UnsortedMap erase test.
///
TEST_F(UnsortedMapTest, Erase) {
  TestMap::Iterator result1 = notempty_.Erase(notempty_.Begin());
  ASSERT_EQ(2, notempty_.Size());
  EXPECT_EQ(notempty_.Begin(), result1);

  int index1 = 0;
  for (TestMap::Iterator it = notempty_.Begin();
      it != notempty_.End(); ++it, index1++) {
    switch (index1) {
      case 0:
        EXPECT_EQ(1, it->first);
        EXPECT_STREQ("test1", it->second.c_str());
        break;
      case 1:
        EXPECT_EQ(2, it->first);
        EXPECT_STREQ("test2", it->second.c_str());
        break;
      default:
        FAIL();
        break;
    }
  }

  TestMap::Iterator result2 = notempty_.Erase(notempty_.Begin() + 1);
  ASSERT_EQ(1, notempty_.Size());
  EXPECT_TRUE(result2 == notempty_.End());

  int index2 = 0;
  for (TestMap::Iterator it = notempty_.Begin();
      it != notempty_.End(); ++it, index2++) {
    switch (index2) {
      case 0:
        EXPECT_EQ(1, it->first);
        EXPECT_STREQ("test1", it->second.c_str());
        break;
      default:
        FAIL();
        break;
    }
  }

  TestMap::Iterator result3 = notempty_.Erase(notempty_.Begin());
  EXPECT_TRUE(notempty_.Empty());
  EXPECT_EQ(0, notempty_.Size());
  EXPECT_TRUE(result3 == notempty_.End());
}

/// pettyutil::UnsortedMap clear test.
///
TEST_F(UnsortedMapTest, Clear) {
  notempty_.Clear();
  EXPECT_TRUE(notempty_.Empty());
  EXPECT_EQ(0, notempty_.Size());
}

/// pettyutil::UnsortedMap find test.
///
TEST_F(UnsortedMapTest, Find) {
  TestMap::Iterator result1 = notempty_.Find(1);
  ASSERT_TRUE(result1 != notempty_.End());
  EXPECT_EQ(1, result1->first);
  EXPECT_STREQ("test1", result1->second.c_str());

  TestMap::Iterator result2 = notempty_.Find(4);
  ASSERT_TRUE(result2 == notempty_.End());
}

/// pettyutil::UnsortedMap const find test.
///
TEST_F(UnsortedMapTest, ConstFind) {
  const TestMap& notempty = notempty_;

  TestMap::ConstIterator result1 = notempty.Find(1);
  ASSERT_TRUE(result1 != notempty.End());
  EXPECT_EQ(1, result1->first);
  EXPECT_STREQ("test1", result1->second.c_str());

  TestMap::ConstIterator result2 = notempty.Find(4);
  ASSERT_TRUE(result2 == notempty.End());
}

/// pettyutil::UnsortedMap count test.
///
TEST_F(UnsortedMapTest, Count) {
  std::size_t result1 = notempty_.Count(1);
  EXPECT_EQ(1, result1);

  std::size_t result2 = notempty_.Count(4);
  EXPECT_EQ(0, result2);
}

}  // namespace
