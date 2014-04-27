#include <duke/attributes/AttributeDisplay.hpp>

#include <duke/base/Slice.hpp>

#include <stdexcept>

namespace attribute {

namespace {

template <typename T>
T as(const void* ptr) {
  return *reinterpret_cast<const T*>(ptr);
}

void display(const Type type, const uint8_t* ptr, const size_t size, StringAppender& appender) {
  switch (type) {
    case Type::Bool:
      appender.append(as<bool>(ptr) ? "true" : "false");
      break;
    case Type::Int8:
      appender.append(std::to_string(as<int8_t>(ptr)));
      break;
    case Type::Int16:
      appender.append(std::to_string(as<int16_t>(ptr)));
      break;
    case Type::Int32:
      appender.append(std::to_string(as<int32_t>(ptr)));
      break;
    case Type::Int64:
      appender.append(std::to_string(as<int64_t>(ptr)));
      break;
    case Type::UInt8:
      appender.append(std::to_string(as<uint8_t>(ptr)));
      break;
    case Type::UInt16:
      appender.append(std::to_string(as<uint16_t>(ptr)));
      break;
    case Type::UInt32:
      appender.append(std::to_string(as<uint32_t>(ptr)));
      break;
    case Type::UInt64:
      appender.append(std::to_string(as<uint64_t>(ptr)));
      break;
    case Type::Float32:
      appender.append(std::to_string(as<float>(ptr)));
      break;
    case Type::Float64:
      appender.append(std::to_string(as<double>(ptr)));
      break;
    case Type::String:
      appender.append('"');
      appender.append(reinterpret_cast<const char*>(ptr));
      appender.append('"');
      break;
    default:
      const auto primitiveType = getPrimitiveForArray(type);
      if (primitiveType == Type::Invalid) {
        appender.append("ERROR");
      } else {
        appender.append('[');
        const auto primitiveSize = getPrimitiveSize(primitiveType);
        CHECK(size % primitiveSize == 0);
        const auto count = size / primitiveSize;
        for (size_t i = 0; i < count && appender; ++i, ptr += primitiveSize) {
          if (i > 0) appender.append(',');
          display(primitiveType, ptr, primitiveSize, appender);
        }
        appender.append(']');
      }
  }
}

}  // namespace

void append(const Attribute& attribute, StringAppender& appender) {
  display(attribute.type, attribute.value.begin(), attribute.value.size(), appender);
}

}  // namespace attribute
