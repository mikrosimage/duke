#pragma once

#include "duke/base/Slice.hpp"

#include <typeindex>
#include <stdexcept>

/**
 * A lightweight object that refers to a static string and a type.
 * It's used by Attribute to store
 */
struct AttributeKey {
  const char* name;
  std::type_index type_index;

  AttributeKey() : name(nullptr), type_index(typeid(void)) {}
  AttributeKey(const char* name, std::type_index type_index)
      : name(name), type_index(type_index) {}

  template <typename T>
  static AttributeKey create(const char* key_name) {
    static_assert(std::is_pod<T>::value, "Type should be a pod");
    return AttributeKey(key_name, typeid(T));
  }

  template <typename T>
  void isSameTypeOrThrow() const {
    const std::type_index requested_type = typeid(T);
    if (requested_type != type_index) {
      char msg[1024];
      snprintf(msg, sizeof(msg), "Requested '%s' but attribute is of type '%s'",
               requested_type.name(), type_index.name());
      throw std::logic_error(msg);
    }
  }
};
