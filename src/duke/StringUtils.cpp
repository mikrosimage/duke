#include "StringUtils.hpp"

bool streq(const char* first, const char* second) {
	if(first==nullptr||second==nullptr)
		return false;
	for (;; ++first, ++second) {
		if (*first != *second)
			return false;
		if (*first == '\0')
			return true;
	}
	return false;
}
