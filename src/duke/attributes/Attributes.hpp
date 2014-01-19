#pragma once

#include <duke/base/SmallVector.hpp>
#include <duke/base/Check.hpp>
#include <duke/base/StringUtils.hpp>

#include <vector>
#include <algorithm>

namespace attribute {

namespace details {

typedef SmallVector<char> AttributeData;

template<class T>
inline AttributeData dematerialize(const T& value) {
    static_assert(std::is_pod<T>::value, "Type has to be Plain Old Data");
    return {reinterpret_cast<const char*>(&value), sizeof(value)};
}

template<>
inline AttributeData dematerialize<const char*>(const char* const & value) {
    return {value, strlen(value) + 1};
}

template<>
inline AttributeData dematerialize<std::string>(const std::string& value) {
    return {value.data(), value.size()};
}

template<typename T>
inline T materialize(const void* ptr, size_t size) {
    CHECK(sizeof(T) == size);
    return *reinterpret_cast<const T*>(ptr);
}

template<>
inline const char* materialize(const void* ptr, size_t size) {
    return reinterpret_cast<const char*>(ptr);
}

template<>
inline std::string materialize<std::string>(const void* ptr, size_t size) {
    return std::string(reinterpret_cast<const char*>(ptr), size);
}

}  // namespace details

}  // namespace attribute

struct Attributes {
private:
    typedef std::tuple<const char*, const void*, attribute::details::AttributeData> MapEntry;
    typedef std::vector<MapEntry> MapImpl;
    typedef MapImpl::const_iterator const_iterator;
    typedef MapImpl::iterator iterator;
    typedef MapImpl::value_type value_type;

    MapImpl m_Map;

    inline static const char*& key(value_type& value) {return std::get<0>(value);}
    inline static const void*& descriptor(value_type& value) {return std::get<1>(value);}
    inline static attribute::details::AttributeData& data(value_type& value) {return std::get<2>(value);}

    inline static const char* key(const value_type& value) {return std::get<0>(value);}
    inline static const void* descriptor(const value_type& value) {return std::get<1>(value);}
    inline static const attribute::details::AttributeData& data(const value_type& value) {return std::get<2>(value);}

    inline const_iterator find(const char* pKey) const {
        return std::find_if(m_Map.begin(), m_Map.end(),
                            [=](const value_type& a) {return streq(key(a), pKey);});
    }

    inline iterator find(const char* pKey) {
        return std::find_if(m_Map.begin(), m_Map.end(),
                            [=](const value_type& a) {return streq(key(a), pKey);});
    }

    inline static bool isValid(const MapEntry& entry) {
        return key(entry);// && descriptor(entry);
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

    const MapEntry& get(const char* pKey) const {
        static const MapEntry empty;
        const auto pFound = find(pKey);
        return pFound == m_Map.end() ? empty : *pFound;
    }

    const MapEntry& getOrDie(const char* pKey) const {
        const MapEntry& entry(get(pKey));
        CHECK(isValid(entry));
        return entry;
    }

    template<typename KEY>
    typename KEY::value_type getOrDie() const {
        using namespace attribute::details;
        const MapEntry& entry(get(KEY().name()));
        CHECK(descriptor(entry) == KEY().descriptor());
        const AttributeData& attributeData = data(entry);
        return materialize<typename KEY::value_type>(attributeData.data(), attributeData.size());
    }

    template<typename KEY>
    typename KEY::value_type getWithDefault(const typename KEY::value_type& default_value) const {
        using namespace attribute::details;
        const MapEntry& entry(get(KEY().name()));
        if (!isValid(entry)) return default_value;
        CHECK(descriptor(entry) == KEY().descriptor());
        const AttributeData& attributeData = data(entry);
        return materialize<typename KEY::value_type>(attributeData.data(), attributeData.size());
    }

    template<typename KEY>
    inline typename KEY::value_type getOrDefault() const {
        return getWithDefault<KEY>(KEY::default_value());
    }

    void set(const char* pKey, const void *pDescriptor, const char* pData, const size_t size) {
        using namespace attribute::details;
        auto pFound = find(pKey);
        if (pFound == end())
            m_Map.emplace_back(pKey, pDescriptor, AttributeData { pData, size });
        else {
            descriptor(*pFound) = pDescriptor;
            data(*pFound) = AttributeData { pData, size };
        }
    }

    template<typename KEY>
    void set(const typename KEY::value_type& value) {
        using namespace attribute::details;
        const char* pKey = KEY().name();
        const auto pFound = find(pKey);
        if (pFound == end())
            m_Map.emplace_back(pKey, KEY().descriptor(), dematerialize<typename KEY::value_type>(value));
        else {
            descriptor(*pFound) = KEY().descriptor();
            data(*pFound) = dematerialize<typename KEY::value_type>(value);
        }
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
