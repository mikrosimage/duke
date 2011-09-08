/*
 * main.cpp
 *
 *  Created on: 14 avr. 2010
 *      Author: Guillaume Chatelet
 */

#include <utils/TimeUtils.h>

#define BOOST_TEST_MODULE ClipHelper
#include <boost/test/unit_test.hpp>

#include <iostream>

using namespace std;

BOOST_AUTO_TEST_SUITE( MetronomSuite )

BOOST_AUTO_TEST_CASE( frameToTimeNTSCTest )
{
	const Rational frameRate( 30000, 1001 );
	const Rational usPerFrame = getMicrosecondsPerFrame( frameRate );

	BOOST_CHECK_EQUAL( Rational( 1001 * 1000000, 30000 ), usPerFrame );
	BOOST_CHECK_EQUAL( ::boost::posix_time::time_duration( 0, 16, 41 ), getTimeFor( 30000, usPerFrame ) );
	BOOST_CHECK_EQUAL( 30000, getFrameFor( ::boost::posix_time::time_duration( 0, 16, 41 ), usPerFrame ) );
}

BOOST_AUTO_TEST_CASE( frameToTimePALTest )
{
	const Rational frameRate( 25 );
	const Rational usPerFrame = getMicrosecondsPerFrame( frameRate );

	BOOST_CHECK_EQUAL( ::boost::posix_time::time_duration( 0, 0, 1 ), getTimeFor( 25, usPerFrame ) );
	BOOST_CHECK_EQUAL( 50, getFrameFor( ::boost::posix_time::time_duration( 0, 0, 2 ), usPerFrame ) );
}

BOOST_AUTO_TEST_SUITE_END()
