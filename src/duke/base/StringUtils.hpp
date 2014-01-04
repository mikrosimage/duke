#pragma once

#include <cstddef>
#include <string>

bool streq(const char* first, const char* second);

/**
 * Returns the number of digits needed to represent frame.
 * ie. digits(5243) == 4
 */
unsigned char digits(size_t frame);

/**
 * Appends frame with padding to output.
 * ie. 12 with padding 4 would result in '0012'
 */
void appendPaddedFrameNumber(const size_t frame, const unsigned char padding, std::string& output);

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
