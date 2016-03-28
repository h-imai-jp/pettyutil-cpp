//
// Copyright 2016 imai hiroyuki.
// All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
#ifndef PETTYUTIL_STRING_UTIL_H_
#define PETTYUTIL_STRING_UTIL_H_

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <cstddef>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#if defined(_MSC_VER) && !defined(va_copy)
# define va_copy(dest, src)  ((dest) = (src))
#endif

namespace pettyutil {

/// String format.
///
/// @param format string format.
/// @param args   argument.
/// @return string.
///
inline std::string StringFormatV(const char* format, va_list args) {
  va_list args_temp;
  va_copy(args_temp, args);
  int length = ::vsnprintf(0, 0, format, args_temp);
  va_end(args_temp);

  if (length > 0) {
    std::vector<char> buffer(length + 1, '\0');
    ::vsnprintf(&buffer[0], buffer.size(), format, args);
    return std::string(&buffer[0]);
  } else {
    return std::string();
  }
}

/// String format.
///
/// @param format string format.
/// @return string.
///
inline std::string StringFormat(const char* format, ...) {
  va_list args;
  va_start(args, format);
  std::string str = StringFormatV(format, args);
  va_end(args);
  return std::move(str);
}

/// String front whitespace length.
///
/// @param str string.
/// @return whitespace length.
///
inline std::string::size_type StringFrontWhitespaceLength(const char* str) {
  std::string::size_type length = 0;

  if (str != nullptr) {
    char character;
    while ((character = str[length]) != '\0') {
      if (((character >= 0x09) && (character <= 0x0d)) || (character == 0x20)) {
        length++;
      } else {
        break;
      }
    }
  }

  return length;
}

/// String back whitespace length.
///
/// @param str string.
/// @return whitespace length.
///
inline std::string::size_type StringBackWhitespaceLength(const char* str) {
  std::size_t length = 0;

  if (str != nullptr) {
    length = ::strlen(str);
    while (length > 0) {
      char character = str[length - 1];
      if (((character >= 0x09) && (character <= 0x0d)) || (character == 0x20)) {
        length--;
      } else {
        break;
      }
    }
  }

  return length;
}

/// String front whitespace trim.
///
/// @param str string.
/// @return trim string.
///
inline std::string StringFrontTrim(const char* str) {
  const char* front_trim = str + StringFrontWhitespaceLength(str);
  return std::string(front_trim);
}

/// String back whitespace trim.
///
/// @param str string.
/// @return trim string.
///
inline std::string StringBackTrim(const char* str) {
  return std::string(str, StringBackWhitespaceLength(str));
}

/// String both ends whitespace trim.
///
/// @param str string.
/// @return trim string.
///
inline std::string StringTrim(const char* str) {
  const char* front_trim = str + StringFrontWhitespaceLength(str);
  return std::string(front_trim, StringBackWhitespaceLength(front_trim));
}

/// Try convert string to integer.
///
/// @param str   numeric string.
/// @param radix radix.
/// @param value output value.
/// @return true if convert successful, otherwise false.
///
template <typename Integer>
inline bool TryStringToInteger(const char* str, int radix, Integer* value) {
  if (str == nullptr) {
    return false;
  }

  // front trim
  str = str + StringFrontWhitespaceLength(str);

  // sign
  Integer sign;
  Integer limit;
  if (*str == '-') {
    if (std::numeric_limits<Integer>::min() == 0) {
      // is unsigned
      return false;
    }
    str++;
    sign = static_cast<Integer>(-1);
    limit = std::numeric_limits<Integer>::max() - 1;
  } else {
    if (*str == '+') {
      str++;
    }
    sign = static_cast<Integer>(1);
    limit = std::numeric_limits<Integer>::max();
  }

  // radix
  if (str[0] == '0') {
    if ((str[1] | 0x20) == 'x') {
      if ((radix == 0) || (radix == 16)) {
        str += 2;
        radix = 16;
      }
    } else if (radix == 0) {
      radix = 8;
    }
  } else if (radix == 0) {
    radix = 10;
  }

  // back trim
  std::size_t length = StringBackWhitespaceLength(str);
  if (length == 0) {
    return false;
  }

  // convert string to integer
  Integer result = 0;
  for (std::size_t i = 0; i < length; i++, str++) {
    Integer digit;

    char code = *str | 0x20;
    if ((*str >= '0') && (*str <= '9')) {
      digit = *str - '0';
    } else if ((code >= 'a') && (code <= 'z')) {
      digit = 10 + (code - 'a');
    } else {
      return false;
    }

    if ((digit >= static_cast<Integer>(radix))
        || (result > ((limit - digit) / static_cast<Integer>(radix)))) {
      return false;
    }

    result = (result * static_cast<Integer>(radix)) + digit;
  }

  if (value != nullptr) {
    *value = result * sign;
  }

  return true;
}

/// Convert string to integer.
///
/// @param str   numeric string.
/// @param radix radix.
/// @return integer value.
///
template <typename Integer>
inline Integer StringToInteger(const char* str, int radix) {
  Integer value;

  if (TryStringToInteger(str, radix, &value)) {
    return value;
  } else {
    return 0;
  }
}

/// Starts with string.
///
/// @param value  value string.
/// @param prefix prefix string.
/// @return ture if value string is starts with prefix string.
///
inline bool StringStartsWith(const std::string& value,
                             const std::string& prefix) {
  return (value.find(prefix) == 0);
}

/// Ends with string.
///
/// @param value  value string.
/// @param suffix suffix string.
/// @return ture if value string is ends with suffix string.
///
inline bool StringEndsWith(const std::string& value,
                           const std::string& suffix) {
  return (suffix.length() <= value.length())
      && std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

/// String replace all.
///
/// @param base       target string.
/// @param old_string string to be replaced.
/// @param new_string string to replace all occurrences of old_string.
///
inline void StringReplaceAll(std::string* base,
                             const std::string& old_string,
                             const std::string& new_string) {
  if (base != 0) {
    std::string::size_type current = 0, found;
    while ((found = base->find(old_string, current)) != std::string::npos) {
      base->replace(found, old_string.length(), new_string);
      current = found + new_string.length();
    }
  }
}

/// String split.
///
/// @param source    string format.
/// @param delimiter delimiter function object.
/// @return split strings.
///
template <class Delimiter>
inline std::vector<std::string> StringSplit(const std::string& source,
                                            Delimiter delimiter) {
  std::vector<std::string> splits;

  std::string::size_type current = 0;
  for (;;) {
    std::string::size_type delimit_length = 1;
    std::string::size_type found = delimiter(source, current, &delimit_length);
    if (found == std::string::npos) {
      break;
    }
    splits.push_back(source.substr(current, found - current));
    current = found + delimit_length;
  }
  splits.push_back(source.substr(current, source.length() - current));

  return std::move(splits);
}

/// Single charactor delimiter function class.
///
class IsAnyOf {
 public:
  /// Construct delimiter.
  ///
  /// @param delimit delimiters.
  ///
  inline explicit IsAnyOf(const std::string& delimit) : delimit_(delimit) {}

  /// Copy delimiter.
  ///
  /// @param value source object.
  ///
  inline IsAnyOf(const IsAnyOf& value) {
    delimit_ = value.delimit_;
  }

  /// Assign delimiter.
  ///
  /// @param value source object.
  ///
  inline void operator=(const IsAnyOf& value) {
    delimit_ = value.delimit_;
  }

  /// Split string function.
  ///
  /// @param source         source string.
  /// @param current        current position.
  /// @param delimit_length output delimiter length.
  /// @return split position.
  ///
  inline std::string::size_type operator()(const std::string& source,
                                           std::string::size_type current,
                                           std::string::size_type*) {
    return source.find_first_of(delimit_, current);
  }

 private:
  /// Delimiters.
  std::string delimit_;
};

/// Convert array to string.
///
/// @param first     source data first.
/// @param last      source data last.
/// @param converter value converter.
/// @return convert string.
///
template <typename InputIterator, class Converter>
inline std::string ArrayToString(InputIterator first,
                                 InputIterator last,
                                 Converter converter) {
  std::string str;

  if (first != last) {
    converter(*first, &str, true);
    while (++first != last) {
      converter(*first, &str, false);
    }
  }

  return std::move(str);
}

/// Hexadecimal byte string converter function class.
///
class ToHexByte {
 public:
  /// Construct converter.
  ///
  /// @param separator separate string.
  /// @param is_upper  is upper character.
  ///
  inline ToHexByte(const std::string& separator, bool is_upper)
      : separator_(separator),
        hex_(is_upper ? 'A' : 'a') {}

  /// Construct converter.
  ///
  /// @param separator separate string.
  ///
  inline explicit ToHexByte(const std::string& separator)
      : separator_(separator),
        hex_('A') {}

  /// Copy converter.
  ///
  /// @param value source object.
  ///
  inline ToHexByte(const ToHexByte& value) {
    separator_ = value.separator_;
    hex_ = value.hex_;
  }

  /// Assign converter.
  ///
  /// @param value source object.
  ///
  inline void operator=(const ToHexByte& value) {
    separator_ = value.separator_;
    hex_ = value.hex_;
  }

  /// Convert string function.
  ///
  /// @param src      source value.
  /// @param dest     destination string.
  /// @param is_first is first.
  ///
  template <typename T>
  void operator()(T src, std::string* dest, bool is_first) {
    if (!is_first) {
      dest->append(separator_);
    }

    char high = (static_cast<char>(src) >> 4) & 0x0f;
    char low = static_cast<char>(src) & 0x0f;
    dest->push_back((high < 10) ? '0' + high : hex_ + (high - 10));
    dest->push_back((low < 10) ? '0' + low : hex_ + (low - 10));
  }

 private:
  /// Separate string.
  std::string separator_;
  /// Base hex character.
  char hex_;
};

/// Multi byte string converter function class.
///
struct ToMultiByte {
  /// Convert string function.
  ///
  /// @param src  source value.
  /// @param dest destination string.
  ///
  template <typename T>
  void operator()(T src, std::string* dest, bool) {
    dest->push_back(static_cast<char>(src & 0xff));
  }
};

}  // namespace pettyutil

#endif  // PETTYUTIL_STRING_UTIL_H_
