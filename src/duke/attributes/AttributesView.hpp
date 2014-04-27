#pragma once

#include "duke/attributes/Attributes.hpp"

namespace attribute {

/**
 * Allows to merge several Attributes.
 */
struct AttributesView {
 private:
  struct Iterator;

 public:
  void merge(const Attributes& attributes);

  void sort();

  inline Iterator begin() const { return Iterator(m_Pointers.begin()); }
  inline Iterator end() const { return Iterator(m_Pointers.end()); }

  inline size_t size() const { return m_Pointers.size(); }
  inline bool empty() const { return m_Pointers.empty(); }

 private:
  struct Iterator {
    Iterator() = default;
    Iterator(const Iterator&) = default;
    Iterator(std::vector<const Attribute*>::const_iterator itr) : m_Itr(itr) {}
    Iterator& operator=(const Iterator&) = default;

    bool operator==(const Iterator& other) const { return m_Itr == other.m_Itr; }
    bool operator!=(const Iterator& other) const { return m_Itr != other.m_Itr; }

    const Attribute* operator->() const { return *m_Itr; }
    const Attribute& operator*() const { return **m_Itr; }
    Iterator& operator++() {
      ++m_Itr;
      return *this;
    }

   private:
    std::vector<const Attribute*>::const_iterator m_Itr;
  };

  std::vector<const Attribute*> m_Pointers;
};

}  // namespace attribute
