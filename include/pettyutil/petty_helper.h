//
// Copyright 2016 imai hiroyuki.
// All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
#ifndef PETTYUTIL_PETTY_HELPER_H_
#define PETTYUTIL_PETTY_HELPER_H_

#include <cstddef>

/// Non-copyable class macro.
/// Macro name is with reference to the Google Style Guides.
///
#ifndef DISALLOW_COPY_AND_ASSIGN
# define DISALLOW_COPY_AND_ASSIGN(TypeName) \
 private: \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)
#endif

namespace pettyutil {

/// Null option.
///
/// @param data   source data.
/// @param option option data.
/// @return if source data is not null, is source data.
///         other case is option data.
///
template <typename T>
inline T NullOption(T data, T option) {
  return (data != nullptr) ? data : option;
}

/// Count of array.
///
template <typename T, std::size_t N>
inline std::size_t CountOf(const T (&)[N]) {
  return N;
}

}  // namespace pettyutil

#endif  // PETTYUTIL_PETTY_HELPER_H_
