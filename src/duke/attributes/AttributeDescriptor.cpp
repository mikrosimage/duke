#include <duke/attributes/AttributeDescriptor.hpp>

template<>
std::string TypedAttributeDescriptor<const char*>::dataToString(const void* pData, size_t size) const {
    return reinterpret_cast<const char*>(pData);
}

template<>
std::string TypedAttributeDescriptor<std::string>::dataToString(const void* pData, size_t size) const {
    return reinterpret_cast<const char*>(pData);
}
