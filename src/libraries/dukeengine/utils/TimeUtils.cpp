/*
 * Metronom.cpp
 *
 *  Created on: 15 avr. 2010
 *      Author: Guillaume Chatelet
 */

#include "TimeUtils.h"

Rational getMicrosecondsPerFrame( const Rational& framerate )
{
	return Rational( 1000000 * framerate.denominator(), framerate.numerator() );
}

::boost::posix_time::time_duration getTimeFor( const size_t frame, const Rational& usPerFrame )
{
	return ::boost::posix_time::microseconds( ::boost::rational_cast<uint64_t>( frame * usPerFrame ) );
}

Rational getFrameFor( const ::boost::posix_time::time_duration& time, const Rational& microsecondsPerFrame )
{
	return time.total_microseconds() / microsecondsPerFrame;
}

