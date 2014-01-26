#pragma once

#include <duke/base/Check.hpp>
#include <duke/base/StringUtils.hpp>
#include <duke/attributes/AttributeEntry.hpp>
#include <duke/attributes/AttributeDescriptor.hpp>

#include <vector>
#include <algorithm>
#include <cstring>

struct Attributes {
private:
    template<class T>
    static inline AttributeData dematerialize(const T& value) {
        static_assert(std::is_pod<T>::value, "Type has to be Plain Old Data");
        const char* pBegin = reinterpret_cast<const char*>(&value);
        return {pBegin, pBegin + sizeof(value)};
    }

    template<typename T>
    static inline T materialize(const void* ptr, size_t size) {
        CHECK(sizeof(T) == size);
        return *reinterpret_cast<const T*>(ptr);
    }

    typedef std::vector<AttributeEntry> MapImpl;
    typedef MapImpl::const_iterator const_iterator;
    typedef MapImpl::iterator iterator;
    typedef MapImpl::value_type value_type;

    MapImpl m_Map;

    inline const_iterator find(const char* pKey) const {
        return std::find_if(m_Map.begin(), m_Map.end(), [=](const value_type& a) {return streq(a.pKey, pKey);});
    }

    inline iterator find(const char* pKey) {
        return std::find_if(m_Map.begin(), m_Map.end(), [=](const value_type& a) {return streq(a.pKey, pKey);});
    }

    inline static bool isValid(const AttributeEntry& entry) {
        return entry.pKey;
    }

public:
    Attributes() {
        m_Map.reserve(16);
    }

    bool contains(const char* pKey) const {
        return find(pKey) != end();
    }

    template<typename KEY>
    inline bool contains() const {
        return contains(KEY().name());
    }

    const AttributeEntry& get(const char* pKey) const {
        static const AttributeEntry empty;
        const auto pFound = find(pKey);
        return pFound == m_Map.end() ? empty : *pFound;
    }

    const AttributeEntry& getOrDie(const char* pKey) const {
        const AttributeEntry& entry(get(pKey));
        CHECK(isValid(entry));
        return entry;
    }

    template<typename KEY>
    inline const AttributeEntry& get() const {
        return get(KEY().name());
    }

    template<typename KEY>
    typename KEY::value_type getOrDie() const {
        const AttributeEntry& entry(get(KEY().name()));
        CHECK(entry.pDescriptor == KEY().descriptor());
        return materialize<typename KEY::value_type>(entry.data.data(), entry.data.size());
    }

    template<typename KEY>
    typename KEY::value_type getWithDefault(const typename KEY::value_type& default_value) const {
        const AttributeEntry& entry(get(KEY().name()));
        if (!isValid(entry)) return default_value;
        CHECK(entry.pDescriptor == KEY().descriptor());
        return materialize<typename KEY::value_type>(entry.data.data(), entry.data.size());
    }

    template<typename KEY>
    inline typename KEY::value_type getOrDefault() const {
        return getWithDefault<KEY>(KEY::default_value());
    }

    void set(const char* pKey, const AttributeDescriptor *pDescriptor, AttributeData&& data) {
        auto pFound = find(pKey);
        if (pFound == end())
            m_Map.emplace_back(pKey, pDescriptor, std::move(data));
        else {
            pFound->pDescriptor = pDescriptor;
            pFound->data = std::move(data);
        }
    }

    void set(const char* pKey, const AttributeDescriptor *pDescriptor, const char* pData, const size_t size) {
        set(pKey, pDescriptor, AttributeData { pData, pData + size });
    }

    template<typename KEY>
    void set(const typename KEY::value_type& value) {
        set(KEY().name(), KEY().descriptor(), dematerialize<typename KEY::value_type>(value));
    }

    void erase(const char* pKey) {
        m_Map.erase(find(pKey));
    }

    template<typename KEY>
    inline void erase() {
        erase(KEY().name());
    }

    inline const const_iterator begin() const {
        return m_Map.begin();
    }
    inline const const_iterator end() const {
        return m_Map.end();
    }
    inline size_t size() const {
        return m_Map.size();
    }
};

template<>
inline AttributeData Attributes::dematerialize<const char*>(const char* const & value) {
    return {value, value + strlen(value) + 1};
}
template<>
inline AttributeData Attributes::dematerialize<std::string>(const std::string& value) {
    return {value.data(), value.data() + value.size() + 1};
}

template<>
inline const char* Attributes::materialize(const void* ptr, size_t size) {
    return reinterpret_cast<const char*>(ptr);
}
template<>
inline std::string Attributes::materialize<std::string>(const void* ptr, size_t size) {
    return std::string(reinterpret_cast<const char*>(ptr), size);
}
