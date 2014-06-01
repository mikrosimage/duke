#include "duke/base/Check.hpp"
#include "duke/base/StringUtils.hpp"
#include "duke/attributes/Attributes.hpp"

#include <algorithm>

namespace attribute {
Attributes::const_iterator find(const Attributes& attributes, const char* pKey) {
  return std::find_if(attributes.begin(), attributes.end(), [pKey](const Attribute& a) { return streq(a.name, pKey); });
};

Attributes::iterator find(Attributes& attributes, const char* pKey) {
  return std::find_if(attributes.begin(), attributes.end(), [pKey](const Attribute& a) { return streq(a.name, pKey); });
};

bool contains(const Attributes& attributes, const char* pKey) { return find(attributes, pKey) != attributes.end(); }

const Attribute& get(const Attributes& attributes, const char* pKey) {
  static const Attribute empty{};
  const auto pFound = find(attributes, pKey);
  return pFound == attributes.end() ? empty : *pFound;
}

const Attribute& getOrDie(const Attributes& attributes, const char* pKey) {
  const Attribute& entry(get(attributes, pKey));
  CHECK(entry.name) << "No attribute '" << pKey << "'";
  return entry;
}

void erase(Attributes& attributes, const char* pKey) {
  auto pFound = find(attributes, pKey);
  if (pFound == attributes.end()) return;
  attributes.erase(pFound);
}

void merge(const Attributes& from, Attributes& to) {
  for (const auto& attribute : from) {
    auto pFound = find(to, attribute.name);
    if (pFound == to.end()) {
      to.push_back(attribute);
    } else {
      *pFound = attribute;
    }
  }
}
}  // namespace attribute
