#pragma once

#include <utility>
#include <limits>
#include <algorithm>

#include <cstring>
#include <cstdlib>
#include <cstdint>

/**
 * A buffer with small size optimization.
 *
 * If size is less or equal to SMALL_SIZE no allocation is performed.
 * Otherwise malloc/free is called to dynamically allocate the memory.
 * The structure is designed to be minimal yet efficient by enabling move
 * construction and assignment.
 */
template <size_t SMALL_SIZE_>
struct SmallBuffer {
  static constexpr size_t SMALL_SIZE = SMALL_SIZE_;
  static_assert(SMALL_SIZE >= sizeof(void *),
                "SmallData should at least be of pointer size, T>=8");

  SmallBuffer() : _size(0) { allocate(); }

  SmallBuffer(size_t size) : _size(size) { allocate(); }

  SmallBuffer(size_t size, const void *ptr) : _size(size) {
    allocateAndCopy(ptr);
  }

  SmallBuffer(const SmallBuffer &other) : _size(other._size) {
    allocateAndCopy(other.begin());
  }

  SmallBuffer &operator=(const SmallBuffer &other) {
    if (_size != other._size) {
      release();
      _size = other._size;
    }
    allocateAndCopy(other.begin());
    return *this;
  }

  SmallBuffer(SmallBuffer &&other) : _size(0) {
    stealContentFrom(std::move(other));
  }

  SmallBuffer &operator=(SmallBuffer &&other) {
    release();
    stealContentFrom(std::move(other));
    return *this;
  }

  ~SmallBuffer() { release(); }

  inline const uint8_t *begin() const { return addressof(); }
  inline uint8_t *begin() { return addressof(); }

  inline const uint8_t *end() const { return begin() + _size; }
  inline uint8_t *end() { return begin() + _size; }

  void fill(const void *ptr) { memcpy(addressof(), ptr, _size); }

  inline size_t size() const { return _size; }

  inline bool empty() const { return _size == 0; }

 private:
  inline void setDynamicPtr(void *ptr) {
    *reinterpret_cast<void **>(_storage) = ptr;
  }

  inline uint8_t *addressof() {
    return isAllocated() ? *reinterpret_cast<uint8_t **>(_storage) : _storage;
  }
  inline const uint8_t *addressof() const {
    return isAllocated() ? *reinterpret_cast<const uint8_t *const *>(_storage)
                         : _storage;
  }

  inline bool isAllocated() const { return _size > SMALL_SIZE; }

  inline void allocate() {
    if (isAllocated()) setDynamicPtr(malloc(_size));
  }

  inline void release() {
    if (isAllocated()) free(addressof());
    _size = 0;
  }

  void allocateAndCopy(const void *ptr) {
    allocate();
    fill(ptr);
  }

  void stealContentFrom(SmallBuffer &&other) {
    memcpy(_storage, other._storage, sizeof(_storage));
    _size = other._size;
    other._size = 0;
  }

  uint8_t _storage[SMALL_SIZE];
  size_t _size;
};
