#pragma once
#include <duke/commands/DescriptorSet.hpp>
#include <string>

enum ParameterType
	:char {
		int_value, float_value, string_value
};

struct ParameterDescription {
	std::string name;
	ParameterType type;
	std::string defaultValue;
};

struct Parameters: public DescriptorSet<ParameterDescription> {
	Parameters();
};
