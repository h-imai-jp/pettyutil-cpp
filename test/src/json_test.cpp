//
// Copyright 2016 imai hiroyuki.
// All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
#include <cstdint>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "pettyutil/json.h"

namespace {

/// thincautility::JsonObject parse test.
///
TEST(JsonObjectTest, Parse) {
  std::unique_ptr<pettyutil::JsonObject> object = pettyutil::JsonObject::Parse("{"
      "\"null\"        : null,"
      "\"true\"        : true,"
      "\"false\"       : false,"
      "\"string\"      : \"test \\u0041\\u0042\\u0043 \\\" \\\\ \\/ \\n\","
      "\"string empty\": \"\","
      "\"number\"      : 12345,"
      "\"number zero\" : 0,"
      "\"object\"      : {\"null\"  : null,"
      "                   \"true\"  : true,"
      "                   \"false\" : false,"
      "                   \"string\": \"string\","
      "                   \"number\": 12345,"
      "                   \"object\": { \"object\" : 0 },"
      "                   \"array\" : [ \"array\" ]},"
      "\"object empty\": {},"
      "\"array\"       : [null,"
      "                   true,"
      "                   false,"
      "                   \"string\","
      "                   12345,"
      "                   { \"object\" : 0 },"
      "                   [ \"array\" ]],"
      "\"array empty\" : []"
      "}");
  ASSERT_TRUE(static_cast<bool>(object));

  EXPECT_TRUE(object->Has("null"));
  EXPECT_FALSE(object->Has("notfound"));

  EXPECT_TRUE(object->IsNull("null"));
  EXPECT_TRUE(object->IsNull("notfound"));
  EXPECT_FALSE(object->IsNull("true"));

  std::unique_ptr<bool> true_value = object->GetBoolean("true");
  ASSERT_TRUE(static_cast<bool>(true_value));
  EXPECT_TRUE(*true_value);
  std::unique_ptr<bool> false_value = object->GetBoolean("false");
  ASSERT_TRUE(static_cast<bool>(false_value));
  EXPECT_FALSE(*false_value);
  EXPECT_FALSE(static_cast<bool>(object->GetBoolean("null")));
  EXPECT_FALSE(static_cast<bool>(object->GetBoolean("notfound")));

  std::unique_ptr<std::string> string_value = object->GetString("string");
  ASSERT_TRUE(static_cast<bool>(string_value));
  EXPECT_STREQ("test ABC \" \\ / \n", string_value->c_str());
  std::unique_ptr<std::string> string_empty_value =
      object->GetString("string empty");
  ASSERT_TRUE(static_cast<bool>(string_empty_value));
  EXPECT_STREQ("", string_empty_value->c_str());
  EXPECT_FALSE(static_cast<bool>(object->GetString("null")));
  EXPECT_FALSE(static_cast<bool>(object->GetString("notfound")));

  std::unique_ptr<std::int32_t> number_value = object->GetInt32("number");
  EXPECT_EQ(12345, *number_value);
  std::unique_ptr<std::int32_t> number_zero_value =
      object->GetInt32("number zero");
  ASSERT_TRUE(static_cast<bool>(number_zero_value));
  EXPECT_EQ(0, *number_zero_value);
  EXPECT_FALSE(static_cast<bool>(object->GetInt32("null")));
  EXPECT_FALSE(static_cast<bool>(object->GetInt32("notfound")));

  const pettyutil::JsonObject* object_value = object->GetObject("object");
  ASSERT_TRUE(object_value != nullptr);
  EXPECT_TRUE(object_value->IsNull("null"));
  EXPECT_TRUE(object_value->OptionBoolean("true", false));
  EXPECT_FALSE(object_value->OptionBoolean("false", true));
  EXPECT_STREQ("string", object_value->OptionString("string", "").c_str());
  EXPECT_EQ(12345, object_value->OptionInt32("number", 0));
  ASSERT_TRUE(object_value->GetObject("object") != nullptr);
  ASSERT_TRUE(object_value->GetArray("array") != nullptr);
  const pettyutil::JsonObject* object_empty_value =
      object->GetObject("object empty");
  EXPECT_TRUE(object_empty_value != nullptr);

  const pettyutil::JsonArray* array_value = object->GetArray("array");
  ASSERT_TRUE(array_value != nullptr);
  EXPECT_TRUE(array_value->IsNull(0));
  EXPECT_TRUE(array_value->OptionBoolean(1, false));
  EXPECT_FALSE(array_value->OptionBoolean(2, true));
  EXPECT_STREQ("string", array_value->OptionString(3, "").c_str());
  EXPECT_EQ(12345, array_value->OptionInt32(4, 0));
  ASSERT_TRUE(array_value->GetObject(5) != nullptr);
  ASSERT_TRUE(array_value->GetArray(6) != nullptr);
  const pettyutil::JsonArray* array_empty_value = object->GetArray("array empty");
  EXPECT_TRUE(array_empty_value != nullptr);
}

}  // namespace
