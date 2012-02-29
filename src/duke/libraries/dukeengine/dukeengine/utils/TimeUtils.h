#ifndef TIMEUTILS_H_
#define TIMEUTILS_H_

#include <boost/rational.hpp>
#include <boost/date_time/local_time/local_time_types.hpp>

typedef ::boost::rational<uint64_t> Rational;

/**
 * Gives the number of nanoseconds a frame has to be displayed
 */
Rational getMicrosecondsPerFrame( const Rational& framerate );

/**
 * Gives the time at which a frame should be displayed
 */
::boost::posix_time::time_duration getTimeFor( const size_t frame, const Rational& microsecondsPerFrame );

/**
 * Gives the frame displayed at a point in time
 */
Rational getFrameFor( const ::boost::posix_time::time_duration& time, const Rational& microsecondsPerFrame );

#endif /* TIMEUTILS_H_ */
