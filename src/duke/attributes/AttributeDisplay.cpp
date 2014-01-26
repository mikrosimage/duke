#include <duke/attributes/AttributeDisplay.hpp>

#include <duke/attributes/Attributes.hpp>

namespace {

const char kNotAvailable[] = "N/A";

}  // namespace

std::string dataString(const AttributeEntry& entry) {
    return entry.pDescriptor ? entry.pDescriptor->dataToString(entry) : kNotAvailable;
}

const char* typeString(const AttributeEntry& entry) {
    return entry.pDescriptor ? entry.pDescriptor->typeToString(entry) : kNotAvailable;
}

const char* nameString(const AttributeEntry& entry) {
    return entry.pDescriptor ? entry.pKey : kNotAvailable;
}
