#pragma once

#include <duke/base/SmallVector.hpp>
#include <duke/base/Check.hpp>

#include <map>

namespace attribute {

namespace details {

typedef SmallVector<char> AttributeData;

template<typename T>
inline T materialize(const void* ptr, size_t size) {
    CHECK(sizeof(T) == size);
    return *reinterpret_cast<const T*>(ptr);
}

template<typename T>
inline AttributeData dematerialize(const T& value) {
    static_assert(std::is_pod<T>::value, "Type has to be Plain Old Data");
    return {reinterpret_cast<const char*>(&value), sizeof(value)};
}

template<>
inline std::string materialize<std::string>(const void* ptr, size_t size) {
    return std::string(reinterpret_cast<const char*>(ptr), size);
}

template<>
inline AttributeData dematerialize<std::string>(const std::string& value) {
    return {value.data(), value.size()};
}

}  // namespace details

struct Attributes {
    template<typename KEY>
    bool contains() const {
        return map_.find(KEY().name()) != map_.end();
    }

    template<typename KEY>
    typename KEY::value_type getOrDie() const {
        const auto pFound = map_.find(KEY().name());
        CHECK(pFound != map_.end());
        return details::materialize<typename KEY::value_type>(pFound->second.data(), pFound->second.size());
    }

    template<typename KEY>
    typename KEY::value_type getWithDefault(const typename KEY::value_type& default_value) const {
        const auto pFound = map_.find(KEY().name());
        return pFound == map_.end() ?
                        default_value :
                        details::materialize<typename KEY::value_type>(pFound->second.data(), pFound->second.size());
    }

    template<typename KEY>
    void set(const typename KEY::value_type& value) {
        map_.emplace(KEY().name(), details::dematerialize<typename KEY::value_type>(value));
    }

    template<typename KEY>
    void erase() {
        map_.erase(KEY().name());
    }

private:
    std::map<const char*, details::AttributeData> map_;
};

}  // namespace attribute
