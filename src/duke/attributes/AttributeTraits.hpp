#pragma once

#include <duke/attributes/AttributeType.hpp>
#include <duke/base/Slice.hpp>
#include <duke/base/Check.hpp>

#include <string>
#include <vector>
#include <initializer_list>

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace attribute {

/**
 * returns the Type associated with a particular C++ type
 */
template <typename T>
constexpr Type runtime_type() {
  return T::unexistent_member;  // this will disallow unresolved specialization
}
#define DEFINE_RUNTIME_TYPE(TYPE, RUNTIME_TYPE) \
  template <>                                   \
  constexpr Type runtime_type<TYPE>() {         \
    return Type::RUNTIME_TYPE;                  \
  }

#define DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(TYPE, RUNTIME_TYPE)                  \
  DEFINE_RUNTIME_TYPE(TYPE, RUNTIME_TYPE);                                           \
  DEFINE_RUNTIME_TYPE(const TYPE, RUNTIME_TYPE);                                     \
  DEFINE_RUNTIME_TYPE(Slice<TYPE>, RUNTIME_TYPE##Array);                             \
  DEFINE_RUNTIME_TYPE(Slice<const TYPE>, RUNTIME_TYPE##Array);                       \
  DEFINE_RUNTIME_TYPE(const Slice<TYPE>, RUNTIME_TYPE##Array);                       \
  DEFINE_RUNTIME_TYPE(const Slice<const TYPE>, RUNTIME_TYPE##Array);                 \
  DEFINE_RUNTIME_TYPE(std::initializer_list<TYPE>, RUNTIME_TYPE##Array);             \
  DEFINE_RUNTIME_TYPE(std::initializer_list<const TYPE>, RUNTIME_TYPE##Array);       \
  DEFINE_RUNTIME_TYPE(const std::initializer_list<TYPE>, RUNTIME_TYPE##Array);       \
  DEFINE_RUNTIME_TYPE(const std::initializer_list<const TYPE>, RUNTIME_TYPE##Array); \
  DEFINE_RUNTIME_TYPE(std::vector<TYPE>, RUNTIME_TYPE##Array);                       \
  DEFINE_RUNTIME_TYPE(std::vector<const TYPE>, RUNTIME_TYPE##Array);                 \
  DEFINE_RUNTIME_TYPE(const std::vector<TYPE>, RUNTIME_TYPE##Array);                 \
  DEFINE_RUNTIME_TYPE(const std::vector<const TYPE>, RUNTIME_TYPE##Array)

DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(bool, Bool);
DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(int8_t, Int8);
DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(int16_t, Int16);
DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(int32_t, Int32);
DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(int64_t, Int64);
DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(uint8_t, UInt8);
DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(uint16_t, UInt16);
DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(uint32_t, UInt32);
DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(uint64_t, UInt64);
DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(float, Float32);
DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE(double, Float64);

DEFINE_RUNTIME_TYPE(const char*, String);
DEFINE_RUNTIME_TYPE(std::string, String);
DEFINE_RUNTIME_TYPE(const std::string, String);

#undef DEFINE_PRIMITIVE_AND_ARRAY_RUNTIME_TYPE
#undef DEFINE_RUNTIME_TYPE

/**
 * Returns the C++ type associated with a particular runtime Type.
 */
template <Type type>
struct return_type {
  typedef void value;
};

#define DEFINE_RETURN_TYPE(TYPE, RUNTIME_TYPE) \
  template <>                                  \
  struct return_type<Type::RUNTIME_TYPE> {     \
    typedef TYPE value;                        \
  }

#define DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(TYPE, RUNTIME_TYPE) \
  DEFINE_RETURN_TYPE(TYPE, RUNTIME_TYPE);                          \
  DEFINE_RETURN_TYPE(Slice<const TYPE>, RUNTIME_TYPE##Array)

DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(bool, Bool);
DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(int8_t, Int8);
DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(int16_t, Int16);
DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(int32_t, Int32);
DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(int64_t, Int64);
DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(uint8_t, UInt8);
DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(uint16_t, UInt16);
DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(uint32_t, UInt32);
DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(uint64_t, UInt64);
DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(float, Float32);
DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE(double, Float64);

DEFINE_RETURN_TYPE(const char*, String);

#undef DEFINE_PRIMITIVE_AND_ARRAY_RETURN_TYPE
#undef DEFINE_RETURN_TYPE

/**
 * Returns whether the runtime Type is an Array
 */
template <Type type>
constexpr bool is_array() {
  return false;
}
template <Type type>
constexpr Type primitive() {
  return Type::Invalid;
}

#define DEFINE_ARRAY_TRAITS(TYPE, ARRAY_TYPE)    \
  template <>                                    \
  constexpr bool is_array<Type::ARRAY_TYPE>() {  \
    return true;                                 \
  }                                              \
  template <>                                    \
  constexpr Type primitive<Type::ARRAY_TYPE>() { \
    return Type::TYPE;                           \
  }

DEFINE_ARRAY_TRAITS(Bool, BoolArray);
DEFINE_ARRAY_TRAITS(Int8, Int8Array);
DEFINE_ARRAY_TRAITS(Int16, Int16Array);
DEFINE_ARRAY_TRAITS(Int32, Int32Array);
DEFINE_ARRAY_TRAITS(Int64, Int64Array);
DEFINE_ARRAY_TRAITS(UInt8, UInt8Array);
DEFINE_ARRAY_TRAITS(UInt16, UInt16Array);
DEFINE_ARRAY_TRAITS(UInt32, UInt32Array);
DEFINE_ARRAY_TRAITS(UInt64, UInt64Array);
DEFINE_ARRAY_TRAITS(Float32, Float32Array);
DEFINE_ARRAY_TRAITS(Float64, Float64Array);

#undef DEFINE_ARRAY_TRAITS

/**
 * Translates a type to a MemorySlice view.
 */

template <typename T>
inline MemorySlice asMemorySlice(const std::vector<T>& value) {
  return {reinterpret_cast<const uint8_t*>(&*value.begin()), reinterpret_cast<const uint8_t*>(&*value.end())};
}

template <typename T>
inline MemorySlice asMemorySlice(const std::initializer_list<T>& value) {
  return {reinterpret_cast<const uint8_t*>(value.begin()), reinterpret_cast<const uint8_t*>(value.end())};
}

template <typename T>
inline MemorySlice asMemorySlice(const Slice<T>& value) {
  return {reinterpret_cast<const uint8_t*>(value.begin()), reinterpret_cast<const uint8_t*>(value.end())};
}

template <typename T, bool = std::is_arithmetic<T>::value>
inline MemorySlice asMemorySlice(const T& value) {
  const uint8_t* pBegin = reinterpret_cast<const uint8_t*>(&value);
  return {pBegin, pBegin + sizeof(T)};
}

inline MemorySlice asMemorySlice(const char* value) {
  const uint8_t* pBegin = reinterpret_cast<const uint8_t*>(value);
  return {pBegin, pBegin + strlen(value) + 1};
}

inline MemorySlice asMemorySlice(const std::string& value) {
  const uint8_t* pBegin = reinterpret_cast<const uint8_t*>(&*value.cbegin());
  return {pBegin, pBegin + value.size() + 1};
}

/**
 * Transform back from a memory Slice to a type.
 */

template <typename T, bool = std::is_arithmetic<T>::value>
inline T asValue(const MemorySlice slice) {
  CHECK(slice.size() == sizeof(T)) << "Corrupt memory";
  return *reinterpret_cast<const T*>(slice.begin());
}

template <typename T>
inline Slice<const T> asArray(const MemorySlice slice) {
  CHECK(slice.size() % sizeof(T) == 0) << "Corrupt memory";
  return Slice<const T>(reinterpret_cast<const T*>(slice.begin()), reinterpret_cast<const T*>(slice.end()));
}

template <>
inline const char* asValue(const MemorySlice slice) {
  return reinterpret_cast<const char*>(slice.begin());
}

template <>
inline std::string asValue(const MemorySlice slice) {
  return reinterpret_cast<const char*>(slice.begin());
}

}  // namespace attribute
