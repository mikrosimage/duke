#include "duke/attributes/AttributeType.hpp"

namespace attribute {

const char* getTypeString(Type type) {
  switch (type) {
    case attribute::Type::Invalid:
      return "Invalid";
    case attribute::Type::Bool:
      return "Bool";
    case attribute::Type::Int8:
      return "Int8";
    case attribute::Type::Int16:
      return "Int16";
    case attribute::Type::Int32:
      return "Int32";
    case attribute::Type::Int64:
      return "Int64";
    case attribute::Type::UInt8:
      return "UInt8";
    case attribute::Type::UInt16:
      return "UInt16";
    case attribute::Type::UInt32:
      return "UInt32";
    case attribute::Type::UInt64:
      return "UInt64";
    case attribute::Type::Float32:
      return "Float32";
    case attribute::Type::Float64:
      return "Float64";
    case attribute::Type::BoolArray:
      return "BoolArray";
    case attribute::Type::Int8Array:
      return "Int8Array";
    case attribute::Type::Int16Array:
      return "Int16Array";
    case attribute::Type::Int32Array:
      return "Int32Array";
    case attribute::Type::Int64Array:
      return "Int64Array";
    case attribute::Type::UInt8Array:
      return "UInt8Array";
    case attribute::Type::UInt16Array:
      return "UInt16Array";
    case attribute::Type::UInt32Array:
      return "UInt32Array";
    case attribute::Type::UInt64Array:
      return "UInt64Array";
    case attribute::Type::Float32Array:
      return "Float32Array";
    case attribute::Type::Float64Array:
      return "Float64Array";
    case attribute::Type::String:
      return "String";
    default:
      return "ERROR";
  }
}

Type getPrimitiveForArray(const Type type) {
  switch (type) {
    case Type::BoolArray:
      return Type::Bool;
    case Type::Int8Array:
      return Type::Int8;
    case Type::Int16Array:
      return Type::Int16;
    case Type::Int32Array:
      return Type::Int32;
    case Type::Int64Array:
      return Type::Int64;
    case Type::UInt8Array:
      return Type::UInt8;
    case Type::UInt16Array:
      return Type::UInt16;
    case Type::UInt32Array:
      return Type::UInt32;
    case Type::UInt64Array:
      return Type::UInt64;
    case Type::Float32Array:
      return Type::Float32;
    case Type::Float64Array:
      return Type::Float64;
    default:
      return Type::Invalid;
  }
}

size_t getPrimitiveSize(const Type type) {
  switch (type) {
    case Type::Bool:
    case Type::Int8:
    case Type::UInt8:
      return 1;
    case Type::Int16:
    case Type::UInt16:
      return 2;
    case Type::Int32:
    case Type::UInt32:
    case Type::Float32:
      return 4;
    case Type::Int64:
    case Type::UInt64:
    case Type::Float64:
      return 8;
    default:
      return 0;
  }
}

}  // namespace attribute
