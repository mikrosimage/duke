#pragma once
#include <cstddef>

namespace attribute {

enum struct Type {
  Invalid,
  Bool,
  Int8,
  Int16,
  Int32,
  Int64,
  UInt8,
  UInt16,
  UInt32,
  UInt64,
  Float32,
  Float64,
  BoolArray,
  Int8Array,
  Int16Array,
  Int32Array,
  Int64Array,
  UInt8Array,
  UInt16Array,
  UInt32Array,
  UInt64Array,
  Float32Array,
  Float64Array,
  String,
  __LAST,
};

const char* getTypeString(Type type);

// Returns Float32 for Float32Array or Invalid for non-Array types
Type getPrimitiveForArray(const Type type);

// Returns the size of an element in bytes
size_t getPrimitiveSize(const Type type);

}  // namespace attribute
