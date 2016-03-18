//
// Copyright 2016 imai hiroyuki.
// All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
#ifndef PETTYUTIL_BASE64_H_
#define PETTYUTIL_BASE64_H_

#include <cstdint>
#include <string>

namespace pettyutil {

/// Base64 charactor table.
const char* kBase64Table =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/// Base64 encoding.
///
/// @param first iterator first.
/// @param last  iterator last.
/// @return base64 string.
///
template <typename InputIterator>
inline std::string EncodeBase64(InputIterator first, InputIterator last) {
  std::string base64;

  while (first != last) {
    base64.push_back(kBase64Table[(*first >> 2) & 0x3f]);
    std::uint8_t remain_bit = (*first << 4) & 0x30;
    if (++first == last) {
      base64.push_back(kBase64Table[remain_bit]);
      base64.append(2, '=');
    } else {
      base64.push_back(kBase64Table[remain_bit | ((*first >> 4) & 0x0f)]);
      remain_bit = (*first << 2) & 0x3c;
      if (++first == last) {
        base64.push_back(kBase64Table[remain_bit]);
        base64.push_back('=');
      } else {
        base64.push_back(kBase64Table[remain_bit | ((*first >> 6) & 0x03)]);
        base64.push_back(kBase64Table[*first & 0x3f]);
        ++first;
      }
    }
  }

  return std::move(base64);
}

/// Check Base64 encoded.
///
/// @param test test string.
/// @return true if base64 encoded string, or false otherwise.
///
bool IsBase64(const std::string& test) {
  if (test.empty() || ((test.length() % 4) != 0)) {
    return false;
  } else {
    std::string::size_type pos = test.find_first_not_of(kBase64Table);
    return (pos == std::string::npos)
        || ((pos >= (test.length() - 2))
            && (test.find_first_not_of('=', pos) == std::string::npos));
  }
}

/// Base64 decoding.
///
/// @param str target string.
/// @return decode data.
///
template <typename OutputIterator>
bool DecodeBase64(const std::string& str, OutputIterator first) {
  if (!IsBase64(str)) {
    return false;
  }

  std::string base64_table = kBase64Table;
  for (std::string::const_iterator it = str.begin(); it != str.end();) {
    std::string::size_type s1 = base64_table.find(*(it++));
    std::string::size_type s2 = base64_table.find(*(it++));
    std::string::size_type s3 = base64_table.find(*(it++));
    std::string::size_type s4 = base64_table.find(*(it++));

    *(first++) = ((s1 & 0x3f) << 2) | ((s2 & 0x30) >> 4);
    if (s3 != std::string::npos) {
      *(first++) = ((s2 & 0x0f) << 4) | ((s3 & 0x3c) >> 2);
      if (s4 != std::string::npos) {
        *(first++) = ((s3 & 0x03) << 6) | ((s4 & 0x3f) >> 0);
      }
    }
  }

  return true;
}

}  // namespace thincautility

#endif  // PETTYUTIL_BASE64_H_
