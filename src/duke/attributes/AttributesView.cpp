#include "duke/attributes/AttributesView.hpp"

#include "duke/base/StringUtils.hpp"

namespace attribute {
void AttributesView::merge(const Attributes& attributes) {
  reserve(size() + attributes.size());
  for (const auto& attribute : attributes) push_back(&attribute);
}

void AttributesView::sort() {
  std::sort(begin(), end(), [](const Attribute* pA, const Attribute* pB) {
    CHECK(pA && pB);
    return strless(pA->name, pB->name);
  });
}

}  // namespace attribute
