#pragma once

#include <string>

struct AttributeEntry;

std::string dataString(const AttributeEntry& entry);

const char* typeString(const AttributeEntry& entry);

const char* nameString(const AttributeEntry& entry);
