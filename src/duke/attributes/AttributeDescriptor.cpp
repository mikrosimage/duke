#include <duke/attributes/AttributeDescriptor.hpp>

template<>
std::string TypedAttributeDescriptor<const char*>::dataToString(const AttributeEntry& entry) const {
    return reinterpret_cast<const char*>(entry.data.data());
}

template<>
std::string TypedAttributeDescriptor<std::string>::dataToString(const AttributeEntry& entry) const {
    return reinterpret_cast<const char*>(entry.data.data());
}
