#include <dukeengine/utils/RangeIterator.h>
#include <dukeengine/utils/TimeUtils.h>

#define BOOST_TEST_MODULE UtilsSequence
#include <boost/test/unit_test.hpp>

using namespace sequence;
using namespace std;

BOOST_AUTO_TEST_SUITE( UtilsTestSuite )

BOOST_AUTO_TEST_CASE( emptyRange ) {
    RangeIterator itr;
    BOOST_CHECK( itr.empty() );
}

BOOST_AUTO_TEST_CASE( oneRange ) {
    Range range(1,1);
    RangeIterator itr(range, 1, FORWARD);
    BOOST_CHECK( !itr.empty() );
    BOOST_CHECK_EQUAL( 1u, itr.front() );
    itr.popFront();
    BOOST_CHECK( itr.empty() );
}

BOOST_AUTO_TEST_CASE( cyclingRange ) {
    Range range(5,7);
    RangeIterator itr(range, 6, FORWARD);
    BOOST_CHECK( !itr.empty() );
    BOOST_CHECK_EQUAL( 6u, itr.front() );
    itr.popFront();
    BOOST_CHECK( !itr.empty() );
    BOOST_CHECK_EQUAL( 7u, itr.front() );
    itr.popFront();
    BOOST_CHECK( !itr.empty() );
    BOOST_CHECK_EQUAL( 5u, itr.front() );
    itr.popFront();
    BOOST_CHECK( itr.empty() );
}

BOOST_AUTO_TEST_SUITE_END()

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
