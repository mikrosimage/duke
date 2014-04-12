#pragma once

#include <cassert>
#include <cstddef>
#include <type_traits>

/**
 * Lightweight object to refer to an immutable array of type T.
 * It does not retain ownership - a so called fat pointer.
 */
template <typename T>
struct Slice {
  typedef typename std::add_const<T>::type value_type;

  value_type* begin() const { return _begin; }
  value_type* end() const { return _end; }
  size_t size() const {
    assert(_end >= _begin);
    return _end - _begin;
  }
  bool empty() const { return _begin == _end; }

  value_type* _begin;
  value_type* _end;
};
