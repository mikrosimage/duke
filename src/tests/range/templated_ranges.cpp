#include <dukeengine/chain/PlaylistRange.h>

#include <iostream>

#define BOOST_TEST_MODULE RangeTest
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace range;

template<typename RANGE>
void checkAndPop(RANGE& itr, typename RANGE::value_type value) {
    BOOST_CHECK_EQUAL( itr.front(), value );
    itr.popFront();
}

BOOST_AUTO_TEST_SUITE( TemplatedRangeSuite )

BOOST_AUTO_TEST_CASE( simple )
{
    SimpleIndexRange itr(0,11);
    for(ptrdiff_t v : {0,1,2,3,4,5,6,7,8,9,10}) {
        checkAndPop( itr, v );
    }
    BOOST_CHECK( itr.empty() );
}

BOOST_AUTO_TEST_CASE( limiter )
{
    Limiter<SimpleIndexRange> itr(SimpleIndexRange(0,11),3);
    for(ptrdiff_t v : {0,1,2}) {
        checkAndPop( itr, v );
    }
    BOOST_CHECK( itr.empty() );
}

BOOST_AUTO_TEST_CASE( alternating )
{
    BalancingIndexRange itr( 0);
    for(ptrdiff_t v : {0,1,-1,2,-2,3,-3,4,-4,5,-5}) {
        checkAndPop( itr, v );
    }
    BOOST_CHECK( !itr.empty() );
}

BOOST_AUTO_TEST_CASE( boundAlternating )
{
    {
        BalancingIndexRange itr( -1);
        for(ptrdiff_t v : {0,1,2,3,4,5,6,7,8,9}) {
            checkAndPop( itr, v );
        }
        BOOST_CHECK( !itr.empty() );
    }
    {
        BalancingIndexRange itr( 1);
        for(ptrdiff_t v : {0,-1,-2,-3,-4,-5,-6,-7,-8,-9}) {
            checkAndPop( itr, v );
        }
        BOOST_CHECK( !itr.empty() );
    }
}

BOOST_AUTO_TEST_CASE( negater )
{
    {
        Reverser<SimpleIndexRange> itr(SimpleIndexRange(0,11),false);
        for(ptrdiff_t v : {0,1,2,3,4,5,6,7,8,9,10}) {
            checkAndPop( itr, v );
        }
        BOOST_CHECK( itr.empty() );
    }
    {
        Reverser<SimpleIndexRange> itr(SimpleIndexRange(0,11));
        for(ptrdiff_t v : {0,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10}) {
            checkAndPop( itr, v );
        }
        BOOST_CHECK( itr.empty() );
    }
    {
        Reverser<BalancingIndexRange> itr(BalancingIndexRange( 0));
        for(ptrdiff_t v : {0,-1,1,-2,2,-3,3,-4,4,-5,5}) {
            checkAndPop( itr, v );
        }
        BOOST_CHECK( !itr.empty() );
    }
}

BOOST_AUTO_TEST_CASE( offset )
{
    {
        OffsetRange<SimpleIndexRange> itr(SimpleIndexRange(0,11),-5);
        for(ptrdiff_t v : {-5,-4,-3,-2,-1,0,1,2,3,4,5}) {
            checkAndPop( itr, v );
        }
        BOOST_CHECK( itr.empty() );
    }
    {
        OffsetRange<BalancingIndexRange> itr(BalancingIndexRange( 0),-5);
        for(ptrdiff_t v : {-5,-4,-6,-3,-7,-2,-8,-1,-9,0,-10}) {
            checkAndPop( itr, v );
        }
        BOOST_CHECK( !itr.empty() );
    }
}

BOOST_AUTO_TEST_CASE( modulo )
{
    {
        ModuloIndexRange<SimpleIndexRange> itr(SimpleIndexRange(0,11),0,1);
        for(ptrdiff_t v : {0,1,0,1,0,1,0,1,0,1,0}) {
            checkAndPop( itr, v );
        }
        BOOST_CHECK( itr.empty() );
    }
    {
        ModuloIndexRange<SimpleIndexRange> itr(SimpleIndexRange(0,11),-1,0);
        for(ptrdiff_t v : {0,-1,0,-1,0,-1,0,-1,0,-1,0}) {
            checkAndPop( itr, v );
        }
        BOOST_CHECK( itr.empty() );
    }
    {
        ModuloIndexRange<SimpleIndexRange> itr(SimpleIndexRange(0,11),0,3);
        for(ptrdiff_t v : {0,1,2,3,0,1,2,3,0,1,2}) {
            checkAndPop( itr, v );
        }
        BOOST_CHECK( itr.empty() );
    }
    {
        ModuloIndexRange<SimpleIndexRange> itr(SimpleIndexRange(0,11),-2,2);
        for(ptrdiff_t v : {0,1,2,-2,-1,0,1,2,-2,-1,0}) {
            checkAndPop( itr, v );
        }
        BOOST_CHECK( itr.empty() );
    }
}

BOOST_AUTO_TEST_CASE( playlistFrameRange0 )
{
    PlaylistFrameRange range(0,1,0,0, false);
    checkAndPop(range, 0 );
}

BOOST_AUTO_TEST_CASE( playlistFrameRange1 )
{
    PlaylistFrameRange range(0,2,0,0, false);

    checkAndPop( range, 0 );
    checkAndPop( range, 1 );
    checkAndPop( range, 2 );
}

BOOST_AUTO_TEST_CASE( playlistFrameRange2 )
{
    BOOST_CHECK_THROW( PlaylistFrameRange(0,2,3,0, false), runtime_error );
    PlaylistFrameRange range(0,2,2,0, false);

    checkAndPop( range, 2 );
    checkAndPop( range, 0 );
    checkAndPop( range, 1 );
}

BOOST_AUTO_TEST_CASE( playlistFrameRange3 )
{
    PlaylistFrameRange range(0,4,2,0, false);

    checkAndPop( range, 2 );
    checkAndPop( range, 3 );
    checkAndPop( range, 1 );
    checkAndPop( range, 4 );
    checkAndPop( range, 0 );
}

BOOST_AUTO_TEST_CASE( playlistFrameRange4 )
{
    { // forward
        PlaylistFrameRange range(0,4,2,-1, false);

        checkAndPop( range, 2 );
        checkAndPop( range, 3 );
        checkAndPop( range, 1 );
        checkAndPop( range, 4 );
        checkAndPop( range, 0 );
    }
    { // reverse
        PlaylistFrameRange range(0,4,2,-1, true);

        checkAndPop( range, 2 );
        checkAndPop( range, 1 );
        checkAndPop( range, 3 );
        checkAndPop( range, 0 );
        checkAndPop( range, 4 );
    }
}

BOOST_AUTO_TEST_CASE( playlistFrameRange5 )
{
    { // forward
        PlaylistFrameRange range(0,4,2,1, false);

        checkAndPop( range, 2 );
        checkAndPop( range, 3 );
        checkAndPop( range, 1 );
        checkAndPop( range, 4 );
        checkAndPop( range, 0 );
    }
    { // reverse
        PlaylistFrameRange range(0,4,2,1, true);

        checkAndPop( range, 2 );
        checkAndPop( range, 1 );
        checkAndPop( range, 3 );
        checkAndPop( range, 0 );
        checkAndPop( range, 4 );
    }
}

BOOST_AUTO_TEST_CASE( playlistFrameRange6 )
{
    // frame outside range
    BOOST_CHECK_THROW(PlaylistFrameRange(0,1,2,0, false), std::runtime_error);
    BOOST_CHECK_THROW(PlaylistFrameRange(0,1,-1,0, false), std::runtime_error);
    // empty
    BOOST_CHECK_THROW(LimitedPlaylistFrameRange(0,0,0,0, false), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( playlistFrameRange7 )
{
    { // reverse
        LimitedPlaylistFrameRange range(0,1,0,0, false);
        checkAndPop( range, 0 );
        checkAndPop( range, 1 );
        BOOST_CHECK( range.empty() );
    }
}

BOOST_AUTO_TEST_SUITE_END()

