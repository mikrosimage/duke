#pragma once

#include "duke/base/Check.hpp"

#include <initializer_list>
#include <vector>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

/**
 * Lightweight object to refer to an array of type T.
 * It does not retain ownership.
 */
template <typename T>
struct Slice {
  typedef T value_type;

  Slice() : _begin(nullptr), _end(nullptr) {}
  Slice(value_type* begin, value_type* end) : _begin(begin), _end(end) {}

  template <int size>
  Slice(value_type (&array)[size])
      : _begin(array), _end(_begin + size) {}

  value_type* begin() { return _begin; }
  value_type* end() { return _end; }
  const value_type* begin() const { return _begin; }
  const value_type* end() const { return _end; }

  value_type& front() { return *_begin; }
  value_type& back() { return *(_end - 1); }
  const value_type front() const { return *_begin; }
  const value_type back() const { return *(_end - 1); }

  size_t size() const {
    assert(_end >= _begin);
    return _end - _begin;
  }

  bool empty() const { return _begin == _end; }

  bool operator==(const Slice& other) const {
    return (_begin == other._begin && _end == other._end) ||
           (size() == other.size() && std::equal(_begin, _end, other._begin));
  }

  bool operator<(const Slice& other) const {
    return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
  }

 private:
  value_type* _begin;
  value_type* _end;
};

template <typename Slice>
Slice pop_front(Slice slice, size_t count = 1) {
  if (count > slice.size()) return Slice();
  return {slice.begin() + count, slice.end()};
}

template <typename Slice>
Slice pop_back(Slice slice, size_t count = 1) {
  if (count > slice.size()) return Slice();
  return {slice.begin(), slice.end() - count};
}

template <typename Slice>
Slice keep_front(Slice slice, size_t count) {
  if (count >= slice.size()) return slice;
  return {slice.begin(), slice.begin() + count};
}

template <typename Slice>
Slice keep_back(Slice slice, size_t count) {
  if (count >= slice.size()) return slice;
  return {slice.end() - count, slice.end()};
}

template <typename T>
Slice<const T> asSlice(const std::initializer_list<T>& array) {
  return Slice<const T>(array.begin(), array.end());
}

template <typename T>
Slice<const T> asSlice(const std::vector<T>& array) {
  return Slice<const T>(array.begin(), array.end());
}

typedef Slice<const char> ConstMemorySlice;
typedef Slice<char> MemorySlice;
