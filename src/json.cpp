//
// Copyright 2016 imai hiroyuki.
// All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
#include "pettyutil/json.h"

#include <stdlib.h>
#if !defined(_MSC_VER) || (_MSC_VER >= 1800)
# include <inttypes.h>
#else
# define PRId32 "I32d"
# define PRId64 "I64d"
#endif
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "pettyutil/string_util.h"

namespace {

// forward declaration
class JsonValue;
class JsonNull;
class JsonBoolean;
class JsonString;
class JsonNumber;
class InputStream;

// function prototype
std::unique_ptr<JsonValue> NumberStringToJsonValue(const std::string& value);
std::unique_ptr<JsonValue> JsonObjectToJsonValue(
    std::unique_ptr<pettyutil::JsonObject> value);
std::unique_ptr<JsonValue> JsonArrayToJsonValue(
    std::unique_ptr<pettyutil::JsonArray> value);
std::unique_ptr<JsonValue> ParseValue(InputStream* input);
std::string SerializeString(const std::string value);

/// JSON value type.
///
enum JsonType {
  /// JSON null.
  kJsonNull,
  /// JSON boolean.
  kJsonBoolean,
  /// JSON string.
  kJsonString,
  /// JSON number.
  kJsonNumber,
  /// JSON object.
  kJsonObject,
  /// JSON array.
  kJsonArray
};

/// JSON value abstract class.
///
class JsonValue {
 public:
  /// Destruct JSON value.
  ///
  virtual ~JsonValue() {}

  /// Get JSON value type.
  ///
  /// @return JSON value type.
  ///
  virtual JsonType GetType() const = 0;

  /// Coerced to boolean value.
  ///
  /// @return boolean value if can be coerced to a boolean,
  ///         or null otherwise.
  ///
  virtual std::unique_ptr<bool> ToBoolean() const {
    return std::unique_ptr<bool>();
  }

  /// Coerced to string value.
  ///
  /// @return string value if can be coerced to a string,
  ///         or null otherwise.
  ///
  virtual std::unique_ptr<std::string> ToString() const {
    return std::unique_ptr<std::string>(new std::string(SerializeValue()));
  }

  /// Coerced to 32bit integer value.
  ///
  /// @return 32bit integer value if can be coerced to a 32bit integer,
  ///         or null otherwise.
  ///
  virtual std::unique_ptr<std::int32_t> ToInt32() const {
    return std::unique_ptr<std::int32_t>();
  }

  /// To object value.
  ///
  /// @return object instance if this class is implemented, or null otherwise.
  ///
  virtual const pettyutil::JsonObject* ToObject() const {
    return nullptr;
  }

  /// To array value.
  ///
  /// @return array instance if this class is implemented, or null otherwise.
  ///
  virtual const pettyutil::JsonArray* ToArray() const {
    return nullptr;
  }

  /// Serialize to JSON value.
  ///
  /// @return serialized JSON string.
  ///
  virtual std::string SerializeValue() const = 0;
};

/// JSON null implement class.
///
class JsonNull : public JsonValue {
 public:
  /// Construct value.
  ///
  JsonNull() {}

  /// Destruct value.
  ///
  virtual ~JsonNull() {}

  /// Implement of const JsonValue::GetType method.
  ///
  JsonType GetType() const override {
    return kJsonNull;
  }

  /// Implement of JsonValue::ToBoolean method.
  ///
  std::unique_ptr<bool> ToBoolean() const override {
    return std::unique_ptr<bool>(new bool(false));
  }

  /// Implement of JsonValue::ToInt32 method.
  ///
  virtual std::unique_ptr<std::int32_t> ToInt32() const {
    return std::unique_ptr<std::int32_t>(new std::int32_t(0));
  }

  /// Implement of const JsonValue::SerializeValue method.
  ///
  std::string SerializeValue() const override {
    return std::string("null");
  }
};

/// JSON boolean implement class.
///
class JsonBoolean : public JsonValue {
 public:
  /// Construct value.
  ///
  /// @param value boolean value.
  ///
  explicit JsonBoolean(bool value) : value_(value) {}

  /// Destruct value.
  ///
  virtual ~JsonBoolean() {}

  /// Implement of const JsonValue::GetType method.
  ///
  JsonType GetType() const override {
    return kJsonBoolean;
  }

  /// Implement of JsonValue::ToBoolean method.
  ///
  std::unique_ptr<bool> ToBoolean() const override {
    return std::unique_ptr<bool>(new bool(value_));
  }

  /// Implement of JsonValue::ToInt32 method.
  ///
  virtual std::unique_ptr<std::int32_t> ToInt32() const {
    return std::unique_ptr<std::int32_t>(new std::int32_t(value_));
  }

  /// Implement of const JsonValue::SerializeValue method.
  ///
  std::string SerializeValue() const override {
    return std::string(value_ ? "true" : "false");
  }

  /// Get boolean value.
  ///
  /// @return boolean value.
  ///
  bool GetBoolean() const {
    return value_;
  }

 private:
  /// Boolean value.
  bool value_;
};

/// JSON string implement class.
///
class JsonString : public JsonValue {
 public:
  /// Construct value.
  ///
  /// @param value string value.
  ///
  explicit JsonString(const std::string& value) : value_(value) {}

  /// Destruct value.
  ///
  virtual ~JsonString() {}

  /// Implement of const JsonValue::GetType method.
  ///
  JsonType GetType() const override {
    return kJsonString;
  }

  /// Implement of JsonValue::ToBoolean method.
  ///
  std::unique_ptr<bool> ToBoolean() const override {
    if (value_.compare("true") == 0) {
      return std::unique_ptr<bool>(new bool(true));
    } else if (value_.compare("false") == 0) {
      return std::unique_ptr<bool>(new bool(false));
    } else {
      return std::unique_ptr<bool>();
    }
  }

  /// Implement of JsonValue::ToString method.
  ///
  std::unique_ptr<std::string> ToString() const override {
    return std::unique_ptr<std::string>(new std::string(value_));
  }

  /// Implement of JsonValue::ToInt32 method.
  ///
  virtual std::unique_ptr<std::int32_t> ToInt32() const {
    if (std::unique_ptr<JsonValue> value = NumberStringToJsonValue(value_)) {
      return value->ToInt32();
    } else {
      return std::unique_ptr<std::int32_t>();
    }
  }

  /// Implement of const JsonValue::SerializeValue method.
  ///
  std::string SerializeValue() const override {
    return SerializeString(value_);
  }

  /// Get string value.
  ///
  const std::string& GetString() const {
    return value_;
  }

 private:
  /// String value.
  std::string value_;
};

/// JSON number implement class.
///
class JsonNumber : public JsonValue {
 public:
  /// Construct 32bit integer.
  ///
  /// @param value 32bit integer value.
  ///
  explicit JsonNumber(std::int32_t value) : type_(kInt32) {
    value_.int32_ = value;
  }

  /// Construct 64bit integer.
  ///
  /// @param value 64bit integer value.
  ///
  explicit JsonNumber(std::int64_t value) : type_(kInt64) {
    value_.int64_ = value;
  }

  /// Construct double.
  ///
  /// @param value double value.
  ///
  explicit JsonNumber(double value) : type_(kDouble) {
    value_.double_ = value;
  }

  /// Destruct value.
  ///
  virtual ~JsonNumber() {}

  /// Implement of const JsonValue::GetType method.
  ///
  JsonType GetType() const override {
    return kJsonNumber;
  }

  /// Implement of JsonValue::ToInt32 method.
  ///
  virtual std::unique_ptr<std::int32_t> ToInt32() const {
    switch (type_) {
      case kInt32:
        return std::unique_ptr<std::int32_t>(new std::int32_t(value_.int32_));
      case kInt64:
        return std::unique_ptr<std::int32_t>(
            new std::int32_t(static_cast<std::int32_t>(value_.int64_)));
      default:  // kDouble
        return std::unique_ptr<std::int32_t>(
            new std::int32_t(static_cast<std::int32_t>(value_.double_)));
    }
  }

  /// Implement of const JsonValue::SerializeValue method.
  ///
  std::string SerializeValue() const override {
    switch (type_) {
      case kInt32:
        return pettyutil::StringFormat("%" PRId32, value_.int32_);
      case kInt64:
        return pettyutil::StringFormat("%" PRId64, value_.int64_);
      default:  // kDouble
        return pettyutil::StringFormat("%g", value_.double_);
    }
  }

 private:
  /// Number value type.
  enum {
    kInt32,
    kInt64,
    kDouble
  } type_;
  /// Number value.
  union {
    /// 32bit integer.
    std::int32_t int32_;
    /// 64bit integer.
    std::int64_t int64_;
    /// double.
    double double_;
  } value_;
};

/// JSON object implement class.
///
class JsonObjectImple : public JsonValue, public pettyutil::JsonObject {
 public:
  /// Construct value.
  ///
  JsonObjectImple() {}

  /// Destruct value.
  ///
  virtual ~JsonObjectImple() {}

  /// Implement of const JsonValue::GetType method.
  ///
  JsonType GetType() const override {
    return kJsonObject;
  }

  /// Implement of JsonValue::ToObject method.
  ///
  const pettyutil::JsonObject* ToObject() const override {
    return this;
  }

  /// Implement of JsonObject::Serialize method.
  ///
  std::string Serialize() const override {
    return SerializeValue();
  }

  /// Implement of JsonObject::Has method.
  ///
  bool Has(const std::string& name) const override {
    return (container_.count(name) != 0);
  }

  /// Implement of JsonObject::IsNull method.
  ///
  bool IsNull(const std::string& name) const override {
    return (!Has(name) || (container_.at(name)->GetType() == kJsonNull));
  }

  /// Implement of JsonObject::GetBoolean method.
  ///
  std::unique_ptr<bool> GetBoolean(const std::string& name) const override {
    if (Has(name) && (container_.at(name)->GetType() == kJsonBoolean)) {
      return container_.at(name)->ToBoolean();
    } else {
      return std::unique_ptr<bool>();
    }
  }

  /// Implement of JsonObject::ToBoolean method.
  ///
  std::unique_ptr<bool> ToBoolean(const std::string& name) const override {
    if (Has(name)) {
      return container_.at(name)->ToBoolean();
    } else {
      return std::unique_ptr<bool>();
    }
  }

  /// Implement of JsonObject::OptionBoolean method.
  ///
  bool OptionBoolean(const std::string& name, bool option) const override {
    if (std::unique_ptr<bool> value = GetBoolean(name)) {
      return *value;
    } else {
      return option;
    }
  }

  /// Implement of JsonObject::GetString method.
  ///
  std::unique_ptr<std::string> GetString(
      const std::string& name) const override {
    if (Has(name) && (container_.at(name)->GetType() == kJsonString)) {
      return container_.at(name)->ToString();
    } else {
      return std::unique_ptr<std::string>();
    }
  }

  /// Implement of JsonObject::ToString method.
  ///
  std::unique_ptr<std::string> ToString(
      const std::string& name) const override {
    if (Has(name)) {
      return container_.at(name)->ToString();
    } else {
      return std::unique_ptr<std::string>();
    }
  }

  /// Implement of JsonObject::OptionString method.
  ///
  std::string OptionString(const std::string& name,
                           const std::string& option) const override {
    if (std::unique_ptr<std::string> value = GetString(name)) {
      return std::move(*value);
    } else {
      return option;
    }
  }

  /// Implement of JsonObject::GetInt32 method.
  ///
  std::unique_ptr<std::int32_t> GetInt32(
      const std::string& name) const override {
    if (Has(name) && (container_.at(name)->GetType() == kJsonNumber)) {
      return container_.at(name)->ToInt32();
    } else {
      return std::unique_ptr<std::int32_t>();
    }
  }

  /// Implement of JsonObject::ToInt32 method.
  ///
  std::unique_ptr<std::int32_t> ToInt32(
      const std::string& name) const override {
    if (Has(name)) {
      return container_.at(name)->ToInt32();
    } else {
      return std::unique_ptr<std::int32_t>();
    }
  }

  /// Implement of JsonObject::OptionInt32 method.
  ///
  std::int32_t OptionInt32(const std::string& name,
                           std::int32_t option) const override {
    if (std::unique_ptr<std::int32_t> value = GetInt32(name)) {
      return *value;
    } else {
      return option;
    }
  }

  /// Implement of JsonObject::GetObject method.
  ///
  const pettyutil::JsonObject* GetObject(const std::string& name) const override {
    return Has(name) ? container_.at(name)->ToObject() : nullptr;
  }

  /// Implement of JsonObject::GetArray method.
  ///
  const pettyutil::JsonArray* GetArray(const std::string& name) const override {
    return Has(name) ? container_.at(name)->ToArray() : nullptr;
  }

  /// Implement of JsonObject::PutNull method.
  ///
  void PutNull(const std::string& name) override {
    container_[name] = std::unique_ptr<JsonValue>(new JsonNull());
  }

  /// Implement of JsonObject::PutBoolean method.
  ///
  void PutBoolean(const std::string& name, bool value) override {
    container_[name] = std::unique_ptr<JsonValue>(new JsonBoolean(value));
  }

  /// Implement of JsonObject::PutString method.
  ///
  void PutString(const std::string& name, const std::string& value) override {
    container_[name] = std::unique_ptr<JsonValue>(new JsonString(value));
  }

  /// Implement of JsonObject::PutInt32 method.
  ///
  void PutInt32(const std::string& name, std::int32_t value) override {
    container_[name] = std::unique_ptr<JsonValue>(new JsonNumber(value));
  }

  /// Implement of JsonObject::PutObject method.
  ///
  void PutObject(const std::string& name,
                 std::unique_ptr<pettyutil::JsonObject> value) override {
    container_[name] = JsonObjectToJsonValue(std::move(value));
  }

  /// Implement of JsonObject::PutArray method.
  ///
  void PutArray(const std::string& name,
                std::unique_ptr<pettyutil::JsonArray> value) override {
    container_[name] = JsonArrayToJsonValue(std::move(value));
  }

  /// Put JSON value.
  ///
  /// @param name  mapping name.
  /// @param value JSON value.
  ///
  void PutValue(const std::string& name, std::unique_ptr<JsonValue> value) {
    container_[name] = std::move(value);
  }

  /// Implement of JsonObject::Remove method.
  ///
  bool Remove(const std::string& name) override {
    return (container_.erase(name) != 0);
  }

  /// Implement of JsonObject::Swap method.
  ///
  void Swap(pettyutil::JsonObject* object) override {
    if (JsonObjectImple* value = static_cast<JsonObjectImple*>(object)) {
      container_.swap(value->container_);
    }
  }

  /// Implement of const JsonValue::SerializeValue method.
  ///
  std::string SerializeValue() const override {
    std::string serial;

    // start character
    serial.push_back('{');

    if (!container_.empty()) {
      for (Container::const_iterator it = container_.begin();
          it != container_.end(); ++it) {
        // object name
        serial.append(SerializeString(it->first));

        // object separator
        serial.push_back(':');

        // object value
        serial.append(it->second->SerializeValue());

        // value separator
        serial.push_back(',');
      }

      // erase unnecessary comma
      serial.erase(serial.size() - 1);
    }

    // end character
    serial.push_back('}');

    return std::move(serial);
  }

 private:
  /// Values container type.
  typedef std::unordered_map<std::string, std::unique_ptr<JsonValue> >
      Container;

  /// Values container.
  Container container_;
};

/// JSON array implement class.
///
class JsonArrayImple : public JsonValue, public pettyutil::JsonArray {
 public:
  /// Construct value.
  ///
  JsonArrayImple() {}

  /// Destruct value.
  ///
  virtual ~JsonArrayImple() {}

  /// Implement of const JsonValue::GetType method.
  ///
  JsonType GetType() const override {
    return kJsonArray;
  }

  /// Implement of JsonValue::ToArray method.
  ///
  const pettyutil::JsonArray* ToArray() const override {
    return this;
  }

  /// Implement of JsonArray::Serialize method.
  ///
  std::string Serialize() const override {
    return SerializeValue();
  }

  /// Implement of JsonArray::GetSize method.
  ///
  std::size_t GetSize() const override {
    return container_.size();
  }

  /// Implement of JsonArray::IsNull method.
  ///
  bool IsNull(std::size_t index) const override {
    return ((index >= GetSize())
        || (container_.at(index)->GetType() == kJsonNull));
  }

  /// Implement of JsonArray::GetBoolean method.
  ///
  std::unique_ptr<bool> GetBoolean(std::size_t index) const override {
    if ((index < GetSize())
        && (container_.at(index)->GetType() == kJsonBoolean)) {
      return container_.at(index)->ToBoolean();
    } else {
      return std::unique_ptr<bool>();
    }
  }

  /// Implement of JsonArray::ToBoolean method.
  ///
  std::unique_ptr<bool> ToBoolean(std::size_t index) const override {
    if (index < GetSize()) {
      return container_.at(index)->ToBoolean();
    } else {
      return std::unique_ptr<bool>();
    }
  }

  /// Implement of JsonArray::OptionBoolean method.
  ///
  bool OptionBoolean(std::size_t index, bool option) const override {
    if (std::unique_ptr<bool> value = GetBoolean(index)) {
      return *value;
    } else {
      return option;
    }
  }

  /// Implement of JsonArray::GetString method.
  ///
  std::unique_ptr<std::string> GetString(std::size_t index) const override {
    if ((index < GetSize())
        && (container_.at(index)->GetType() == kJsonString)) {
      return container_.at(index)->ToString();
    } else {
      return std::unique_ptr<std::string>();
    }
  }

  /// Implement of JsonArray::ToString method.
  ///
  std::unique_ptr<std::string> ToString(std::size_t index) const override {
    if (index < GetSize()) {
      return container_.at(index)->ToString();
    } else {
      return std::unique_ptr<std::string>();
    }
  }

  /// Implement of JsonArray::OptionString method.
  ///
  std::string OptionString(std::size_t index,
                           const std::string& option) const override {
    if (std::unique_ptr<std::string> value = GetString(index)) {
      return std::move(*value);
    } else {
      return option;
    }
  }

  /// Implement of JsonArray::GetInt32 method.
  ///
  std::unique_ptr<std::int32_t> GetInt32(std::size_t index) const override {
    if ((index < GetSize())
        && (container_.at(index)->GetType() == kJsonNumber)) {
      return container_.at(index)->ToInt32();
    } else {
      return std::unique_ptr<std::int32_t>();
    }
  }

  /// Implement of JsonArray::ToInt32 method.
  ///
  std::unique_ptr<std::int32_t> ToInt32(std::size_t index) const override {
    if (index < GetSize()) {
      return container_.at(index)->ToInt32();
    } else {
      return std::unique_ptr<std::int32_t>();
    }
  }

  /// Implement of JsonArray::OptionInt32 method.
  ///
  std::int32_t OptionInt32(std::size_t index,
                           std::int32_t option) const override {
    if (std::unique_ptr<std::int32_t> value = GetInt32(index)) {
      return *value;
    } else {
      return option;
    }
  }

  /// Implement of JsonArray::GetObject method.
  ///
  const pettyutil::JsonObject* GetObject(std::size_t index) const override {
    return (index < GetSize()) ? container_.at(index)->ToObject() : nullptr;
  }

  /// Implement of JsonArray::GetArray method.
  ///
  const pettyutil::JsonArray* GetArray(std::size_t index) const override {
    return (index < GetSize()) ? container_.at(index)->ToArray() : nullptr;
  }

  /// Implement of JsonArray::AppendNull method.
  ///
  void AppendNull() override {
    container_.push_back(std::unique_ptr<JsonValue>(new JsonNull()));
  }

  /// Implement of JsonArray::AppendBoolean method.
  ///
  void AppendBoolean(bool value) override {
    container_.push_back(std::unique_ptr<JsonValue>(new JsonBoolean(value)));
  }

  /// Implement of JsonArray::AppendString method.
  ///
  void AppendString(const std::string& value) override {
    container_.push_back(std::unique_ptr<JsonValue>(new JsonString(value)));
  }

  /// Implement of JsonArray::AppendInt32 method.
  ///
  void AppendInt32(std::int32_t value) override {
    container_.push_back(std::unique_ptr<JsonValue>(new JsonNumber(value)));
  }

  /// Implement of JsonArray::AppendObject method.
  ///
  void AppendObject(std::unique_ptr<pettyutil::JsonObject> value) override {
    container_.push_back(JsonObjectToJsonValue(std::move(value)));
  }

  /// Implement of JsonArray::AppendArray method.
  ///
  void AppendArray(std::unique_ptr<pettyutil::JsonArray> value) override {
    container_.push_back(JsonArrayToJsonValue(std::move(value)));
  }

  /// Append JSON value.
  ///
  /// @param value JSON value.
  ///
  void AppendValue(std::unique_ptr<JsonValue> value) {
    container_.push_back(std::move(value));
  }

  /// Implement of JsonArray::AppendArray method.
  ///
  bool Remove(std::size_t index) override {
    if (index < GetSize()) {
      container_.erase(container_.begin() + index);
      return true;
    } else {
      return false;
    }
  }

  /// Implement of JsonArray::Swap method.
  ///
  void Swap(pettyutil::JsonArray* arr) override {
    if (JsonArrayImple* value = static_cast<JsonArrayImple*>(arr)) {
      container_.swap(value->container_);
    }
  }

  /// Implement of const JsonValue::SerializeValue method.
  ///
  std::string SerializeValue() const override {
    std::string serial;

    // start character
    serial.push_back('[');

    if (!container_.empty()) {
      for (Container::const_iterator it = container_.begin();
          it != container_.end(); ++it) {
        // array value
        serial.append((*it)->SerializeValue());

        // value separator
        serial.push_back(',');
      }

      // erase unnecessary comma
      serial.erase(serial.size() - 1);
    }

    // end character
    serial.push_back(']');

    return std::move(serial);
  }

 private:
  /// Values container type.
  typedef std::vector<std::unique_ptr<JsonValue> > Container;

  /// Values container.
  Container container_;
};

/// Input stream class.
///
class InputStream {
 public:
  /// Construct stream.
  ///
  InputStream(std::string::const_iterator begin,
              std::string::const_iterator end)
      : begin_(begin),
        end_(end) {}

  /// Get character.
  ///
  /// @return character.
  ///
  char GetChar() {
    return (begin_ != end_) ? *begin_ : 0;
  }

  /// Get next character.
  ///
  /// @return next character.
  ///
  char NextChar() {
    return (begin_ != end_) ? *(++begin_) : 0;
  }

  /// Get token character.
  ///
  /// @return token character.
  ///
  char GetToken() {
    Trim();
    return GetChar();
  }

  /// Advancing the next position.
  ///
  bool Next() {
    if (begin_ == end_) {
      return false;
    } else {
      ++begin_;
      return true;
    }
  }

  /// Compare word and advancing the next position.
  ///
  /// @param word compare word.
  /// @return true if match word.
  ///
  bool CompareAndNext(const std::string& word) {
    for (std::string::const_iterator it = word.begin();
        it != word.end(); ++it, ++begin_) {
      if (*it != GetChar()) {
        return false;
      }
    }

    return true;
  }

  /// Trim whitespace.
  ///
  void Trim() {
    while (begin_ != end_) {
      if (((*begin_ >= 0x09) && (*begin_ <= 0x0d)) || (*begin_ == 0x20)) {
        ++begin_;
      } else {
        break;
      }
    }
  }

 private:
  /// Stream begin.
  std::string::const_iterator begin_;
  /// Stream end.
  std::string::const_iterator end_;
};

/// Number string to JSON value.
///
/// @param value number string.
/// @return JSON value.
///
std::unique_ptr<JsonValue> NumberStringToJsonValue(const std::string& value) {
  std::int32_t int32_value = 0;
  std::int64_t int64_value = 0;

  std::string::size_type index = value.find_first_not_of(' ');
  if ((index == std::string::npos) || (value.at(index) == '+')) {
    return std::unique_ptr<JsonValue>();
  }

  if (pettyutil::TryStringToInteger(value.c_str(), 10, &int32_value)) {
    // to 32bit integer
    return std::unique_ptr<JsonValue>(new JsonNumber(int32_value));
  } else if (pettyutil::TryStringToInteger(value.c_str(), 10, &int64_value)) {
    // to 64bit integer
    return std::unique_ptr<JsonValue>(new JsonNumber(int64_value));
  } else {
    // to double
    char* end_point = nullptr;
    double double_value = ::strtod(value.c_str(), &end_point);
    if ((end_point != nullptr) && (*end_point == '\0')
        && (double_value != HUGE_VAL) && (double_value != -HUGE_VAL)) {
      return std::unique_ptr<JsonValue>(new JsonNumber(double_value));
    } else {
      return std::unique_ptr<JsonValue>();
    }
  }
}

/// JSON object to JSON value.
///
/// @param value JSON object.
/// @return JSON value.
///
std::unique_ptr<JsonValue> JsonObjectToJsonValue(
    std::unique_ptr<pettyutil::JsonObject> value) {
  return std::unique_ptr<JsonValue>(
      static_cast<JsonObjectImple*>(value.release()));
}

/// JSON array to JSON value.
///
/// @param value JSON array.
/// @return JSON value.
///
std::unique_ptr<JsonValue> JsonArrayToJsonValue(
    std::unique_ptr<pettyutil::JsonArray> value) {
  return std::unique_ptr<JsonValue>(
      static_cast<JsonArrayImple*>(value.release()));
}

/// Parse 4 hexadecimal digits.
///
bool Parse4HexadecimalDigits(InputStream* input, std::uint16_t* output) {
  std::uint16_t value = 0;

  for (int i = 0; i < 4; i++) {
    char hex_char = input->NextChar();
    if ((hex_char >= '0') && (hex_char <= '9')) {
      value = (value << 4) + (hex_char - '0');
    } else if ((hex_char >= 'A') && (hex_char <= 'F')) {
      value = (value << 4) + (hex_char - 'A');
    } else if ((hex_char >= 'a') && (hex_char <= 'f')) {
      value = (value << 4) + (hex_char - 'a');
    } else {
      return false;
    }
  }

  *output = value;
  return true;
}

/// Parse code point.
///
bool ParseCodePoint(InputStream* input, std::string* output) {
  std::uint16_t code = 0;
  if (!Parse4HexadecimalDigits(input, &code)) {
    return false;
  }

  if ((code >= 0xd800) && (code <= 0xdbff)) {
    // high surrogate code
    std::uint16_t code_low = 0;
    if ((input->NextChar() != '\\') || (input->NextChar() != 'u')
        || !Parse4HexadecimalDigits(input, &code_low)) {
      return false;
    }
    if ((code_low >= 0xdc00) && (code_low <= 0xdfff)) {
      // low surrogate code
      code = ((code & 0x03ff) << 10) + (code_low & 0x03ff) + 0x10000;
    } else {
      return false;
    }
  } else if ((code >= 0xdc00) && (code <= 0xdfff)) {
    // low surrogate code
    return false;
  }

  if (code < 0x0080) {
    output->push_back(static_cast<char>(code));
  } else if (code < 0x0800) {
    output->push_back(0xc0 | (code >> 6));
    output->push_back(0x80 | (code & 0x3f));
  } else {
    output->push_back(0xe0 | (code >> 12));
    output->push_back(0x80 | ((code >> 6) & 0x3f));
    output->push_back(0x80 | (code & 0x3f));
  }

  return true;
}

/// Parse JSON string.
///
/// @param input input stream.
/// @return JSON value.
///
std::unique_ptr<JsonString> ParseString(InputStream* input) {
  // start character
  if (input->GetToken() != '"') {
    return std::unique_ptr<JsonString>();
  }

  std::string value;
  char character;
  while ((character = input->NextChar()) != '"') {
    if (((character >= 0x00) && (character <= 0x1f)) || (character == 0x7f)) {
      // control character
      return std::unique_ptr<JsonString>();
    } else if (character == '\\') {
      // escape character
      switch (input->NextChar()) {
        case '"':
          value.push_back('"');
          break;
        case '\\':
          value.push_back('\\');
          break;
        case '/':
          value.push_back('/');
          break;
        case 'b':
          value.push_back('\b');
          break;
        case 'f':
          value.push_back('\f');
          break;
        case 'n':
          value.push_back('\n');
          break;
        case 'r':
          value.push_back('\r');
          break;
        case 't':
          value.push_back('\t');
          break;
        case 'u':
          if (!ParseCodePoint(input, &value)) {
            return std::unique_ptr<JsonString>();
          }
          break;
        default:
          return std::unique_ptr<JsonString>();
      }
    } else {
      value.push_back(character);
    }
  }

  if ((input->GetChar() == '"') && input->Next()) {
    // end character
    return std::unique_ptr<JsonString>(new JsonString(value));
  } else {
    return std::unique_ptr<JsonString>();
  }
}

/// Parse JSON number.
///
/// @param input input stream.
/// @return JSON number.
///
std::unique_ptr<JsonValue> ParseNumber(InputStream* input) {
  // trim whitespace
  input->Trim();

  // number value
  std::string value;
  do {
    char character = input->GetChar();
    if (((character >= '0') && (character <= '9'))
        || (character == '+') || (character == '-')
        || (character == '.') || (character == 'e') || (character == 'E')) {
      value.push_back(character);
    } else {
      break;
    }
  } while (input->Next());

  return NumberStringToJsonValue(value);
}

/// Parse JSON object.
///
/// @param input input stream.
/// @return JSON object.
///
std::unique_ptr<JsonObjectImple> ParseObject(InputStream* input) {
  // start character
  if ((input->GetToken() != '{') || !input->Next()) {
    return std::unique_ptr<JsonObjectImple>();
  }

  std::unique_ptr<JsonObjectImple> json_object(new JsonObjectImple());
  if (input->GetToken() != '}') {
    do {
      // object name
      std::unique_ptr<JsonString> name = ParseString(input);
      if (!name) {
        return std::unique_ptr<JsonObjectImple>();
      }

      // object separator
      if ((input->GetToken() != ':') || !input->Next()) {
        return std::unique_ptr<JsonObjectImple>();
      }

      // object value
      std::unique_ptr<JsonValue> value = ParseValue(input);
      if (!value) {
        return std::unique_ptr<JsonObjectImple>();
      }

      json_object->PutValue(name->GetString(), std::move(value));
    } while ((input->GetToken() == ',') && input->Next());
  }

  if ((input->GetChar() == '}') && input->Next()) {
    // end character
    return std::move(json_object);
  } else {
    return std::unique_ptr<JsonObjectImple>();
  }
}

/// Parse JSON array.
///
/// @param input input stream.
/// @return JSON array.
///
std::unique_ptr<JsonArrayImple> ParseArray(InputStream* input) {
  // start character
  if ((input->GetToken() != '[') || !input->Next()) {
    return std::unique_ptr<JsonArrayImple>();
  }

  std::unique_ptr<JsonArrayImple> json_array(new JsonArrayImple());
  if (input->GetToken() != ']') {
    do {
      // array value
      if (std::unique_ptr<JsonValue> value = ParseValue(input)) {
        json_array->AppendValue(std::move(value));
      } else {
        return std::unique_ptr<JsonArrayImple>();
      }
    } while ((input->GetToken() == ',') && input->Next());
  }

  if ((input->GetChar() == ']') && input->Next()) {
    // end character
    return std::move(json_array);
  } else {
    return std::unique_ptr<JsonArrayImple>();
  }
}

/// Parse JSON value.
///
/// @param input input stream.
/// @return JSON value.
///
std::unique_ptr<JsonValue> ParseValue(InputStream* input) {
  switch (input->GetToken()) {
    case 'n':   // null
      if (input->CompareAndNext("null")) {
        return std::unique_ptr<JsonValue>(new JsonNull());
      } else {
        return std::unique_ptr<JsonValue>();
      }
    case 't':   // true
      if (input->CompareAndNext("true")) {
        return std::unique_ptr<JsonValue>(new JsonBoolean(true));
      } else {
        return std::unique_ptr<JsonValue>();
      }
    case 'f':   // false
      if (input->CompareAndNext("false")) {
        return std::unique_ptr<JsonValue>(new JsonBoolean(false));
      } else {
        return std::unique_ptr<JsonValue>();
      }
    case '"':   // string
      return ParseString(input);
    case '{':   // object
      return ParseObject(input);
    case '[':   // array
      return ParseArray(input);
    default:    // number
      return ParseNumber(input);
  }
}

/// Serialize JSON string.
///
/// @param value string value.
/// @return serialized JSON string.
///
std::string SerializeString(const std::string value) {
  std::string serial;

  serial.push_back('"');
  for (std::string::const_iterator it = value.begin();
      it != value.end(); ++it) {
    switch (*it) {
      case '"':
        serial.append("\\\"");
        break;
      case '\\':
        serial.append("\\\\");
        break;
      case '/':
        serial.append("\\/");
        break;
      case '\b':
        serial.append("\\b");
        break;
      case '\f':
        serial.append("\\f");
        break;
      case '\n':
        serial.append("\\n");
        break;
      case '\r':
        serial.append("\\r");
        break;
      case '\t':
        serial.append("\\t");
        break;
      default:
        serial.push_back(*it);
        break;
    }
  }
  serial.push_back('"');

  return std::move(serial);
}

}  // namespace

namespace pettyutil {

/// Parse from JSON string.
///
std::unique_ptr<JsonObject> JsonObject::Parse(const std::string& json) {
  InputStream input(json.begin(), json.end());
  return ParseObject(&input);
}

/// Parse from JSON string.
///
std::unique_ptr<JsonArray> JsonArray::Parse(const std::string& json) {
  InputStream input(json.begin(), json.end());
  return ParseArray(&input);
}

}  // namespace pettyutil
