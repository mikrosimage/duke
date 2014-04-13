#pragma once

#include "duke/base/Slice.hpp"
#include "duke/base/SmallBuffer.hpp"
#include "duke/attributes/AttributeKey.hpp"

#include <algorithm>
#include <string>
#include <vector>

/**
 * A small value that can efficiently transport POD (plain old data) or arrays
 * of POD in a type safe manner.  If data is small no allocation is performed,
 * this ensures good cache locality and good performance.
 *
 * An Attribute is made of :
 * - a key which stores the attribute name and type.
 * - an untyped data which stores the value.
 */
struct Attribute {
  typedef SmallBuffer<8> ValueType;
  AttributeKey key;
  ValueType value;

  /**
   * Setters
   */
  template <typename T>
  void set(const char* name, const T& value) {
    setData<T>(name, &value, sizeof(value));
  }
  template <typename T>
  void set(const char* name, const std::initializer_list<T>& vector) {
    setData(name, vector);
  }
  template <typename T>
  void set(const char* name, const std::vector<T>& vector) {
    setData(name, vector);
  }
  template <typename T>
  void set(const char* name, const Slice<T>& vector) {
    setData(name, vector);
  }
  void set(const char* name, const std::string& string) {
    setData<Slice<char>>(name, string.c_str(), string.size() + 1);
  }
  void set(const char* name, const char* string) {
    setData<Slice<char>>(name, string, strlen(string) + 1);
  }

  /**
   * Getters
   */
  template <typename T>
  const T& asValue() const {
    key.isSameTypeOrThrow<T>();
    return *reinterpret_cast<const T*>(value.begin());
  }
  template <typename T>
  Slice<T> asSlice() const {
    key.isSameTypeOrThrow<Slice<T>>();
    return Slice<T>{reinterpret_cast<const T*>(value.begin()),
                    reinterpret_cast<const T*>(value.end())};
  }
  // Prefer asSlice as this method will allocate and copy the buffer.
  template <typename T>
  std::vector<T> asVector() const {
    key.isSameTypeOrThrow<Slice<T>>();
    return std::vector<T>(reinterpret_cast<const T*>(value.begin()),
                          reinterpret_cast<const T*>(value.end()));
  }
  const char* asCString() const {
    key.isSameTypeOrThrow<Slice<char>>();
    return reinterpret_cast<const char*>(value.begin());
  }
  // Prefer asSlice or asCString as this method will allocate and copy the
  // buffer.
  std::string asString() const {
    return {asCString()};
  }

 private:
  typedef Slice<uint8_t> ByteSlice;

  template <typename T>
  void setData(const char* name, const void* begin, size_t size) {
    key = AttributeKey::create<T>(name);
    const ByteSlice slice = {reinterpret_cast<const uint8_t*>(begin),
                             reinterpret_cast<const uint8_t*>(begin) + size};
    value = ValueType(slice.size(), slice.begin());
  }

  template <typename Container>
  void setData(const char* name, const Container& container) {
    typedef typename Container::value_type cv_value_type;
    typedef typename std::remove_const<cv_value_type>::type value_type;
    static_assert(std::is_pod<value_type>::value,
                  "vector element type should be a Plain Old Data");
    setData<Slice<value_type>>(
        name, reinterpret_cast<const void*>(&*container.begin()),
        container.size() * sizeof(value_type));
  }
};

#ifdef __LP64__
static_assert(sizeof(Attribute) != 64,
              "Attribute should be 64B large on x86_64");
#endif
