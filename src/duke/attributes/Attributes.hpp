#pragma once

#include <duke/attributes/Attribute.hpp>

#include <vector>

namespace attribute {

typedef std::vector<Attribute> Attributes;

Attributes::const_iterator find(const Attributes& attributes, const char* pKey);

Attributes::iterator find(Attributes& attributes, const char* pKey);

bool contains(const Attributes& attributes, const char* pKey);

const Attribute& get(const Attributes& attributes, const char* pKey);

const Attribute& getOrDie(const Attributes& attributes, const char* pKey);

void erase(Attributes& attributes, const char* pKey);

template <typename TYPE>
void set(Attributes& attributes, const char* key, TYPE&& data) {
  auto pFound = find(attributes, key);
  if (pFound == attributes.end()) {
    attributes.emplace_back(key, std::forward<TYPE>(data));
  } else {
    *pFound = Attribute(key, std::forward<TYPE>(data));
  }
}

template <typename TYPE>
bool setIfNotPresent(Attributes& attributes, const char* key, TYPE&& data) {
  auto pFound = find(attributes, key);
  if (pFound == attributes.end()) {
    attributes.emplace_back(key, std::forward<TYPE>(data));
    return true;
  }
  return false;
}

}  // namespace attribute
