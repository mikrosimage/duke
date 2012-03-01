#include <dukeapi/protobuf_builder/PlaylistBuilder.h>
#include <dukeengine/utils/PlaylistIterator.h>
#include <dukeengine/utils/RangeIterator.h>
#include <dukeengine/utils/TimeUtils.h>

#define BOOST_TEST_MODULE UtilsSequence
#include <boost/test/unit_test.hpp>

using namespace duke::protocol;
using namespace sequence;
using namespace std;

void check(const PlaylistIndex &a, const PlaylistIndex &b) {
    BOOST_CHECK_EQUAL(a.frame, b.frame);
    BOOST_CHECK_EQUAL(a.track, b.track);
}

void checkAndPop(RangeIterator& itr, unsigned int value) {
    BOOST_CHECK( !itr.empty());
    BOOST_CHECK_EQUAL( value, itr.front());
    itr.popFront();
}

BOOST_AUTO_TEST_SUITE( RangeIteratorTestSuite )

BOOST_AUTO_TEST_CASE( emptyRange ) {
    RangeIterator itr;
    BOOST_CHECK( itr.empty() );
}

BOOST_AUTO_TEST_CASE( oneRange ) {
    Range range(0,0);
    RangeIterator itr(range, 0, FORWARD);
    checkAndPop(itr, 0);
    BOOST_CHECK( itr.empty() );
}

BOOST_AUTO_TEST_CASE( cyclingRange ) {
    Range range(5,7);
    {
        RangeIterator itr(range, 6, FORWARD);
        checkAndPop(itr, 6);
        checkAndPop(itr, 7);
        checkAndPop(itr, 5);
        BOOST_CHECK( itr.empty() );
    }
    {
        RangeIterator itr(range, 6, REVERSE);
        checkAndPop(itr, 6);
        checkAndPop(itr, 5);
        checkAndPop(itr, 7);
        BOOST_CHECK( itr.empty() );
    }
}

BOOST_AUTO_TEST_CASE( cyclingRangeBalance ) {
    Range range(1,9);
    RangeIterator itr(range, 6, BALANCE);
    checkAndPop(itr, 6);
    checkAndPop(itr, 7);
    checkAndPop(itr, 5);
    checkAndPop(itr, 8);
    checkAndPop(itr, 4);
    checkAndPop(itr, 9);
    checkAndPop(itr, 3);
    checkAndPop(itr, 1);
    checkAndPop(itr, 2);
    BOOST_CHECK( itr.empty() );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( PlaylistIteratorTestSuite )

PlaylistHelper build() {
    PlaylistBuilder builder;
    TrackBuilder track = builder.addTrack("default");
    track.addSequence("file-####.jpg", sequence::Range(0,10), sequence::Range(0,10));
    return PlaylistHelper(builder);
}

BOOST_AUTO_TEST_CASE( emptyPlaylist ) {
    {
        PlaylistIterator itr;
        BOOST_CHECK(itr.empty());
    }
    {
        PlaylistHelper helper;
        PlaylistIterator itr(helper, FORWARD, helper.range.first, helper.range);
        BOOST_CHECK(itr.empty());
    }
    {
        PlaylistBuilder builder;
        TrackBuilder track = builder.addTrack("default");
        PlaylistHelper helper(builder);
        PlaylistIterator itr(helper, FORWARD, helper.range.first, helper.range);
        BOOST_CHECK(itr.empty());
    }
    {
        PlaylistBuilder builder;
        TrackBuilder track = builder.addTrack("default");
        track.addSequence("file-####.jpg", sequence::Range(0,0), sequence::Range(0,0));
        PlaylistHelper helper(builder);
        PlaylistIterator itr(helper, FORWARD, helper.range.first, helper.range);
        BOOST_CHECK(!itr.empty());
    }
}

BOOST_AUTO_TEST_CASE( __first ) {
    PlaylistHelper helper(build());
    PlaylistIterator itr(helper, FORWARD, 5, helper.range);

    BOOST_CHECK(!itr.empty());
    MediaFrame frame = itr.front();
    check( PlaylistIndex(5,0) , frame.index );
    BOOST_CHECK_EQUAL( 5 , frame.source );

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
