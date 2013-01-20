/*
 * StringUtils.cpp
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#include "StringUtils.h"

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
