#pragma once

bool streq(const char* first, const char* second);

#include <functional>
struct nocase_compare: public std::binary_function<unsigned char, unsigned char, bool> {
	bool operator()(const unsigned char& c1, const unsigned char& c2) const;
};

/**
 * Case insensitive string comparator
 */
#include <string>
struct ci_less: std::binary_function<std::string, std::string, bool> {
	bool operator()(const std::string & s1, const std::string & s2) const;
};
