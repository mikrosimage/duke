/*
 * Durations.h
 *
 *  Created on: 30 avr. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef DURATIONS_H_
#define DURATIONS_H_

#include "boost/date_time/posix_time/posix_time_types.hpp"

struct Durations
{
	::boost::posix_time::time_duration m_ImageLoad;
	::boost::posix_time::time_duration m_ImageDecode;
	::boost::posix_time::time_duration m_WholeRendering;
};

#endif /* DURATIONS_H_ */
