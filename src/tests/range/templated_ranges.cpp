#include <dukeengine/range/PlaylistRange.h>

#include <iostream>

#include <boost/foreach.hpp>

#define BOOST_TEST_MODULE RangeTest
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace range;

#define CHECK_AND_POP(itr,value) BOOST_CHECK_EQUAL( itr.front(), value );itr.popFront()
#define CHECK_EMPTY(itr) BOOST_CHECK( itr.empty() )
#define CHECK_NOT_EMPTY(itr) BOOST_CHECK( !itr.empty() )

BOOST_AUTO_TEST_SUITE( TemplatedRangeSuite )

BOOST_AUTO_TEST_CASE( forward )
{
    UnlimitedForwardRange itr;
    const ptrdiff_t array[] {0,1,2,3,4,5,6,7,8,9,10};
    BOOST_FOREACH(ptrdiff_t v , array) {
        CHECK_AND_POP( itr, v );
    }
    CHECK_NOT_EMPTY(itr);
}

BOOST_AUTO_TEST_CASE( limiter )
{
    Limiter<UnlimitedForwardRange> itr(UnlimitedForwardRange(),3);
    const ptrdiff_t array[] {0,1,2};
    BOOST_FOREACH(ptrdiff_t v , array) {
        CHECK_AND_POP( itr, v );
    }
    CHECK_EMPTY(itr);
}

BOOST_AUTO_TEST_CASE( alternating )
{
    BalancingRange itr;
    const ptrdiff_t array[] {0,1,-1,2,-2,3,-3,4,-4,5,-5};
    BOOST_FOREACH(ptrdiff_t v , array) {
        CHECK_AND_POP( itr, v );
    }
    CHECK_NOT_EMPTY(itr);
}

BOOST_AUTO_TEST_CASE( offset )
{
    {
        OffsetRange<UnlimitedForwardRange> itr(UnlimitedForwardRange(),-5);
        const ptrdiff_t array[] {-5,-4,-3,-2,-1,0,1,2,3,4,5};
        BOOST_FOREACH(ptrdiff_t v , array) {
            CHECK_AND_POP( itr, v );
        }
        CHECK_NOT_EMPTY(itr);
    }
    {
        OffsetRange<BalancingRange> itr(BalancingRange(),-5);
        const ptrdiff_t array[] {-5,-4,-6,-3,-7,-2,-8,-1,-9,0,-10};
        BOOST_FOREACH(ptrdiff_t v , array) {
            CHECK_AND_POP( itr, v );
        }
        CHECK_NOT_EMPTY(itr);
    }
}

BOOST_AUTO_TEST_CASE( modulo )
{
    {
        ModuloIndexRange<UnlimitedForwardRange> itr(UnlimitedForwardRange(),0,1);
        const ptrdiff_t array[] {0,1,0,1,0,1,0,1,0,1,0};
        BOOST_FOREACH(ptrdiff_t v , array) {
            CHECK_AND_POP( itr, v );
        }
        CHECK_NOT_EMPTY(itr);
    }
    {
        ModuloIndexRange<UnlimitedForwardRange> itr(UnlimitedForwardRange(),-1,0);
        const ptrdiff_t array[] {0,-1,0,-1,0,-1,0,-1,0,-1,0};
        BOOST_FOREACH(ptrdiff_t v , array) {
            CHECK_AND_POP( itr, v );
        }
        CHECK_NOT_EMPTY(itr);
    }
    {
        ModuloIndexRange<UnlimitedForwardRange> itr(UnlimitedForwardRange(),0,3);
        const ptrdiff_t array[] {0,1,2,3,0,1,2,3,0,1,2};
        BOOST_FOREACH(ptrdiff_t v , array ) {
            CHECK_AND_POP( itr, v );
        }
        CHECK_NOT_EMPTY(itr);
    }
    {
        ModuloIndexRange<UnlimitedForwardRange> itr(UnlimitedForwardRange(),-2,2);
        const ptrdiff_t array[] {0,1,2,-2,-1,0,1,2,-2,-1,0};
        BOOST_FOREACH(ptrdiff_t v , array) {
            CHECK_AND_POP( itr, v );
        }
        CHECK_NOT_EMPTY(itr);
    }
}

BOOST_AUTO_TEST_CASE( limitedForwardPlaylist )
{
    LimitedPlaylistFrameRange range(0,1,0,1);
    CHECK_AND_POP(range, 0 );
    CHECK_AND_POP(range, 1 );
    CHECK_EMPTY(range);
}

BOOST_AUTO_TEST_CASE( limitedForwardPlaylistLooping )
{
    LimitedPlaylistFrameRange range(0,2,2,1);

    CHECK_AND_POP( range, 2 );
    CHECK_AND_POP( range, 0 );
    CHECK_AND_POP( range, 1 );
    CHECK_EMPTY(range);
}

BOOST_AUTO_TEST_CASE( limitedStoppedPlaylist )
{
    LimitedPlaylistFrameRange range(0,4,2,0);

    CHECK_AND_POP( range, 2 );
    CHECK_AND_POP( range, 3 );
    CHECK_AND_POP( range, 1 );
    CHECK_AND_POP( range, 4 );
    CHECK_AND_POP( range, 0 );
    CHECK_EMPTY(range);
}

BOOST_AUTO_TEST_CASE( limitedBackwardPlaylist )
{
    LimitedPlaylistFrameRange range(0,4,2,-1);

    CHECK_AND_POP( range, 2 );
    CHECK_AND_POP( range, 1 );
    CHECK_AND_POP( range, 0 );
    CHECK_AND_POP( range, 4 );
    CHECK_AND_POP( range, 3 );
    CHECK_EMPTY(range);
}

BOOST_AUTO_TEST_CASE( frameOutsideRange )
{
    BOOST_CHECK_THROW(PlaylistFrameRange(0,1,2,1), std::runtime_error);
    BOOST_CHECK_THROW(PlaylistFrameRange(0,1,-1,1), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()

