//
// Copyright 2016 imai hiroyuki.
// All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
#ifndef PETTYUTIL_UNSORTED_MAP_H_
#define PETTYUTIL_UNSORTED_MAP_H_

#include <iterator>
#include <utility>
#include <vector>

namespace pettyutil {

/// Unsorted map class.
///
template <typename Key, typename T>
class UnsortedMap {
 public:
  /// Unsorted map iterator.
  ///
  typedef typename std::vector<std::pair<Key, T> >::iterator Iterator;

  /// Unsorted map const iterator.
  ///
  typedef typename std::vector<std::pair<Key, T> >::const_iterator
      ConstIterator;

  /// Unsorted map size type.
  ///
  typedef typename std::vector<std::pair<Key, T> >::size_type SizeType;

  /// Unsorted map value type.
  ///
  typedef typename std::vector<std::pair<Key, T> >::value_type ValueType;

  /// Construct unsorted map.
  ///
  UnsortedMap() {}

  /// Return iterator to beginning.
  ///
  Iterator Begin() {
    return map_.begin();
  }

  /// Return const iterator to beginning.
  ///
  ConstIterator Begin() const {
    return map_.begin();
  }

  /// Return iterator to end.
  ///
  Iterator End() {
    return map_.end();
  }

  /// Return const iterator to end.
  ///
  ConstIterator End() const {
    return map_.end();
  }

  /// Test whether container is empty.
  ///
  bool Empty() const {
    return map_.empty();
  }

  /// Return container size.
  ///
  SizeType Size() const {
    return map_.size();
  }

  /// Access element.
  ///
  T& operator[](const Key& key) {
    return Insert(End(), std::make_pair(key, T()))->second;
  }

  /// Insert elements.
  ///
  std::pair<Iterator, bool> Insert(const ValueType& value) {
    Iterator it = Find(value.first);
    if (it == End()) {
      return std::pair<Iterator, bool>(map_.insert(it, value), true);
    } else {
      return std::pair<Iterator, bool>(it, false);
    }
  }

  /// Insert elements.
  ///
  Iterator Insert(Iterator position, const ValueType& value) {
    Iterator it = Find(value.first);
    if (it == End()) {
      it = map_.insert(position, value);
    }

    return it;
  }

  /// Erase elements.
  ///
  Iterator Erase(Iterator position) {
    return map_.erase(position);
  }

  /// Clear content.
  ///
  void Clear() {
    map_.clear();
  }

  /// Get iterator to element.
  ///
  Iterator Find(const Key& key) {
    for (Iterator it = Begin(); it != End(); ++it) {
      if (it->first == key) {
        return it;
      }
    }

    return End();
  }

  /// Get const iterator to element.
  ///
  ConstIterator Find(const Key& key) const {
    for (ConstIterator it = Begin(); it != End(); ++it) {
      if (it->first == key) {
        return it;
      }
    }

    return End();
  }

  /// Count elements with a specific key.
  ///
  SizeType Count(const Key& key) const {
    return (Find(key) != End()) ? 1 : 0;
  }

 private:
  /// Unsorted map container.
  std::vector<std::pair<Key, T> > map_;
};

}  // namespace pettyutil

#endif  // PETTYUTIL_UNSORTED_MAP_H_
