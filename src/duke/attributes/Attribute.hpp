#pragma once

#include "duke/base/Slice.hpp"
#include "duke/base/SmallBuffer.hpp"
#include "duke/attributes/AttributeType.hpp"
#include "duke/attributes/AttributeTraits.hpp"

namespace attribute {

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
  typedef SmallBuffer<40> value_type;  // 40 Bytes + sizeof(size) = 48 Bytes
  const char* name = nullptr;          // 8  Bytes
  Type type = Type::Invalid;           // 4  Bytes but laid out in 8 Bytes
  value_type value;                    // 48 Bytes

  Attribute() : name(nullptr), type(Type::Invalid) {}
  Attribute(const Attribute&) = default;
  Attribute& operator=(const Attribute&) = default;
  Attribute(Attribute&& other)
      : name(std::move(other.name)), type(std::move(other.type)), value(std::move(other.value)) {
    other = Attribute();
  }
  Attribute& operator=(Attribute&& other) {
    name = std::move(other.name);
    type = std::move(other.type);
    value = std::move(other.value);
    other.name = nullptr;
    other.type = Type::Invalid;
    return *this;
  }

  Attribute(const char* name, const char* value)
      : name(name), type(attribute::runtime_type<const char*>()), value(asMemorySlice(value)) {}

  template <int size>
  Attribute(const char* name, const char (&value)[size])
      : name(name), type(attribute::runtime_type<const char*>()), value(asMemorySlice(value)) {}

  template <typename T>
  Attribute(const char* name, const T& value)
      : name(name), type(attribute::runtime_type<T>()), value(asMemorySlice(value)) {}

  operator ConstMemorySlice() const {
    return {value.begin(), value.end()};
  }

  bool operator==(const Attribute& other) const {
    return name == other.name && type == other.type && value == other.value;
  }
};

#ifdef __LP64__
static_assert(sizeof(Type) == 4, "Type should be 4B large on x86_64");
static_assert(sizeof(Attribute::value_type) == 48, "Attribute::ValueType should be 48B large on x86_64");
static_assert(sizeof(Attribute) == 64, "Attribute should be 64B large on x86_64");
#endif

}  // namespace attribute
