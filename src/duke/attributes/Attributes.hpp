#pragma once

#include <duke/base/SmallVector.hpp>
#include <duke/base/Check.hpp>

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
    typedef std::vector<std::pair<const char*, attribute::details::AttributeData>> MapImpl;
    typedef MapImpl::const_iterator const_iterator;
    typedef MapImpl::iterator iterator;
    typedef MapImpl::value_type value_type;

    MapImpl m_Map;

    template<typename KEY>
    inline const_iterator find() const {
        return std::find_if(m_Map.begin(), m_Map.end(), [](const value_type& a) {return a.first == KEY().name();});
    }

    template<typename KEY>
    inline iterator find() {
        return std::find_if(m_Map.begin(), m_Map.end(), [](const value_type& a) {return a.first == KEY().name();});
    }

public:
    Attributes() { m_Map.reserve(16); }

    template<typename KEY>
    bool contains() const {
        return find<KEY>() != end();
    }

    template<typename KEY>
    typename KEY::value_type getOrDie() const {
        const auto pFound = find<KEY>();
        CHECK(pFound != end());
        return attribute::details::materialize<typename KEY::value_type>(pFound->second.data(), pFound->second.size());
    }

    template<typename KEY>
    typename KEY::value_type getWithDefault(const typename KEY::value_type& default_value) const {
        const auto pFound = find<KEY>();
        return pFound == end() ? default_value : attribute::details::materialize<typename KEY::value_type>(pFound->second.data(), pFound->second.size());
    }

    template<typename KEY>
    void set(const typename KEY::value_type& value) {
        const auto pFound = find<KEY>();
        if (pFound == end())
            m_Map.emplace_back(KEY().name(), attribute::details::dematerialize<typename KEY::value_type>(value));
        else
            pFound->second = attribute::details::dematerialize<typename KEY::value_type>(value);
    }

    template<typename KEY>
    void erase() {
        m_Map.erase(find<KEY>());
    }

    inline const const_iterator begin() const { return m_Map.begin(); }
    inline const const_iterator end() const { return m_Map.end(); }
    inline size_t size() const { return m_Map.size(); }
};
