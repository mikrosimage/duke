#include "StringUtils.hpp"

bool streq(const char* first, const char* second) {
    if (first == second)
        return first; // same pointer or both nullptr
	for (;; ++first, ++second) {
		if (*first != *second)
			return false;
		if (*first == '\0')
			return true;
	}
	return false;
}

bool nocase_compare::operator()(const unsigned char& c1, const unsigned char& c2) const {
	return tolower(c1) < tolower(c2);
}

bool ci_less::operator()(const std::string & s1, const std::string & s2) const {
	return std::lexicographical_compare(s1.begin(), s1.end(),   // source range
			s2.begin(), s2.end(),   // dest range
			nocase_compare());  // comparison
}
