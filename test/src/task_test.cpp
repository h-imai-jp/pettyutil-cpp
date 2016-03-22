//
// Copyright 2016 imai hiroyuki.
// All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
#include <memory>
#include <string>

#include "gtest/gtest.h"
#include "pettyutil/task.h"

namespace {

/// Test task class.
///
class TestTask : public pettyutil::Runnable {
 public:
  TestTask(std::string* all_message, const std::string& message)
      : all_message_(all_message),
        message_(message) {}
  virtual ~TestTask() {}

  void Run() override {
    all_message_->append(message_);
  }

 private:
  std::string* all_message_;
  std::string message_;
};

/// pettyutil::Task construct test.
///
TEST(TaskTest, Construct) {
  std::string message;
  pettyutil::Task test(std::make_shared<TestTask>(&message, "1"));
  test.Run();
  EXPECT_STREQ("1", message.c_str());
}

/// pettyutil::Task continue test.
///
TEST(TaskTest, ContinueWith) {
  std::string message;
  pettyutil::Task test(std::make_shared<TestTask>(&message, "1"));
  test.ContinueWith(std::make_shared<TestTask>(&message, "2"))
      ->ContinueWith(std::make_shared<TestTask>(&message, "3"));
  test.ContinueWith(std::make_shared<TestTask>(&message, "4"))
      ->ContinueWith(std::make_shared<TestTask>(&message, "5"))
      ->ContinueWith(std::make_shared<TestTask>(&message, "6"));
  test.Run();
  EXPECT_STREQ("145623", message.c_str());
}

/// pettyutil::Task swap test.
///
TEST(TaskTest, Swap) {
  std::string message;
  pettyutil::Task test1(std::make_shared<TestTask>(&message, "1"));
  {
    pettyutil::Task test2(std::make_shared<TestTask>(&message, "2"));
    test2.ContinueWith(std::make_shared<TestTask>(&message, "3"));
    test1.Swap(&test2);
  }
  test1.Run();
  EXPECT_STREQ("23", message.c_str());
}

/// pettyutil::Task MakeTask test.
///
TEST(TaskTest, MakeTask) {
  std::string message;
  auto test1 = pettyutil::MakeTask(new TestTask(&message, "1"));
  {
    auto test2 = pettyutil::MakeTask(new TestTask(&message, "2"));
    test2->ContinueWith(std::make_shared<TestTask>(&message, "3"))
        ->ContinueWith(test1);
    test1.swap(test2);
  }
  test1->Run();
  EXPECT_STREQ("231", message.c_str());
}

}  // namespace
