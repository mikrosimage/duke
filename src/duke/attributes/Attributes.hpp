#pragma once

#include <duke/attributes/Attribute.hpp>
#include <duke/base/Check.hpp>
#include <duke/base/StringUtils.hpp>

#include <algorithm>
#include <vector>
#include <type_traits>

struct Attributes : public std::vector<Attribute> {
 private:
  inline const_iterator find(const char* pKey) const {
    return std::find_if(begin(), end(), [=](const value_type& a) {
      return streq(a.key.name, pKey);
    });
  }

  inline iterator find(const char* pKey) {
    return std::find_if(begin(), end(), [=](const value_type& a) {
      return streq(a.key.name, pKey);
    });
  }

  inline static bool isValid(const Attribute& entry) { return entry.key.name; }

 public:
  bool contains(const char* pKey) const { return find(pKey) != end(); }

  template <typename PROTOTYPE>
  inline bool contains() const {
    return contains(PROTOTYPE::key());
  }

  const Attribute& get(const char* pKey) const {
    static const Attribute empty {};
    const auto pFound = find(pKey);
    return pFound == end() ? empty : *pFound;
  }

  const Attribute& getOrDie(const char* pKey) const {
    const Attribute& entry(get(pKey));
    CHECK(isValid(entry));
    return entry;
  }

  template <typename PROTOTYPE>
  inline const Attribute& get() const {
    return get(PROTOTYPE::key());
  }

  template <typename PROTOTYPE>
  typename PROTOTYPE::value_type getOrDie() const {
    const Attribute& entry(get(PROTOTYPE::key()));
    CHECK(streq(entry.key.name, PROTOTYPE::key()));
    return PROTOTYPE::get(entry);
  }

  template <typename PROTOTYPE>
  typename PROTOTYPE::value_type getWithDefault(
      const typename PROTOTYPE::value_type& default_value) const {
    const Attribute& entry(get(PROTOTYPE::key()));
    if (!isValid(entry)) return default_value;
    CHECK(streq(entry.key.name, PROTOTYPE::key()));
    return PROTOTYPE::get(entry);
  }

  template <typename PROTOTYPE>
  inline typename PROTOTYPE::value_type getOrDefault() const {
    return getWithDefault<PROTOTYPE>(PROTOTYPE::default_value());
  }

  template <typename TYPE>
  void set(const char* key, TYPE&& data) {
    auto pFound = find(key);
    if (pFound == end()) {
      emplace_back();
      back().set(key, data);
    } else {
      pFound->set(key, data);
    }
  }
  template <typename PROTOTYPE, typename TYPE = typename PROTOTYPE::value_type>
  void set(TYPE&& value) {
    set(PROTOTYPE::key(), std::forward<TYPE>(value));
  }

  void erase(const char* pKey) { std::vector<Attribute>::erase(find(pKey)); }

  template <typename PROTOTYPE>
  inline void erase() {
    erase(PROTOTYPE::key());
  }
};

#define RegisterValueAttribute(TYPE, NAME, DEFAULT)                  \
  struct NAME {                                                      \
    typedef TYPE value_type;                                         \
    static_assert(std::is_pod<TYPE>::value, "Type should be a pod"); \
    static constexpr const char* const key() { return #NAME; }       \
    static constexpr value_type default_value() { return DEFAULT; }  \
    static value_type get(const Attribute& attribute) {              \
      return attribute.asValue<value_type>();                        \
    }                                                                \
  }

#define RegisterArrayAttribute(TYPE, NAME, DEFAULT)                        \
  struct NAME {                                                            \
    typedef Slice<TYPE> value_type;                                        \
    static_assert(std::is_pod<value_type>::value, "Type should be a pod"); \
    static constexpr const char* const key() { return #NAME; }             \
    static constexpr value_type default_value() { return DEFAULT; }        \
    static value_type get(const Attribute& attribute) {                    \
      return attribute.asSlice<TYPE>();                                    \
    }                                                                      \
  }

#define RegisterStringAttribute(NAME, DEFAULT)                      \
  struct NAME {                                                     \
    typedef const char* value_type;                                 \
    static constexpr const char* const key() { return #NAME; }      \
    static constexpr value_type default_value() { return DEFAULT; } \
    static value_type get(const Attribute& attribute) {             \
      return attribute.asCString();                                 \
    }                                                               \
  }
