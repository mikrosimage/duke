#pragma once

#include "duke/attributes/Attributes.hpp"

namespace attribute {

/**
 * Allows to merge several Attributes.
 */
struct AttributesView : public std::vector<const Attribute*> {
  void merge(const Attributes& attributes);
  void sort();
};

}  // namespace attribute
