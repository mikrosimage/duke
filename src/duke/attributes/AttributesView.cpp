#include "duke/attributes/AttributesView.hpp"

#include "duke/base/StringUtils.hpp"

namespace attribute {
void AttributesView::merge(const Attributes& attributes) {
  m_Pointers.reserve(m_Pointers.size() + attributes.size());
  for (const auto& attribute : attributes) m_Pointers.push_back(&attribute);
}

void AttributesView::sort() {
  std::sort(m_Pointers.begin(), m_Pointers.end(), [](const Attribute* pA, const Attribute* pB) {
    CHECK(pA && pB);
    return strless(pA->name, pB->name);
  });
}

}  // namespace attribute
