#include <dukeengine/cache/PriorityCache.hpp>

#define BOOST_TEST_MODULE CacheTestModule
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace cache;

typedef PriorityCache<size_t, size_t, int> CACHE;

BOOST_AUTO_TEST_SUITE( CacheTestSuite )

BOOST_AUTO_TEST_CASE( cacheFullness )
{
    BOOST_ASSERT( CACHE(0).isFull() );
    BOOST_ASSERT( ! CACHE(1).isFull() );
}

BOOST_AUTO_TEST_CASE( cacheBasics )
{
    CACHE cache(10);
    BOOST_ASSERT( ! cache.isPending(0) );
    BOOST_ASSERT( ! cache.contains(0) );
    BOOST_CHECK_EQUAL( cache.update(0), NEEDED ); // creating
    BOOST_CHECK_EQUAL( cache.update(0), NOT_NEEDED ); // creating
    BOOST_ASSERT( cache.isPending(0) );// now pending
    BOOST_ASSERT( ! cache.contains(0) );// but still not present
    BOOST_ASSERT( cache.put(0,1,-1) );// putting
    BOOST_ASSERT( cache.contains(0) );// now present
    BOOST_CHECK_EQUAL( cache.cacheSize(),1 );// now present

    CACHE::data_type data;
    BOOST_ASSERT( cache.get(0, data) );// getting is ok
    BOOST_CHECK_EQUAL( data, -1 );// data is correct
}

BOOST_AUTO_TEST_CASE( noWeight )
{
    CACHE cache(1);
    BOOST_CHECK_THROW( cache.put(0,0,-1), std::logic_error ); // no weight
}

BOOST_AUTO_TEST_CASE( putEvenIfNotRequestedCanFit )
{
    CACHE cache(1);
    // putting an unneeded element
    BOOST_ASSERT( cache.put(5,1,-1) );
    BOOST_ASSERT( cache.contains(5) );// now present

    // a new request would be priority and discard the previous one
    cache.update(0);// requesting 0
    BOOST_ASSERT( cache.put(0,1,2) );// should be accepted
    BOOST_ASSERT( cache.contains(0) );// should be present
    BOOST_ASSERT( ! cache.contains(5) );// should be discarded
}

BOOST_AUTO_TEST_CASE( putEvenIfNotRequestedButCantFit )
{
    CACHE cache(1);
    cache.update(0); // requesting 0
    BOOST_ASSERT( cache.put(0,2,2) );// should be accepted
    BOOST_ASSERT( cache.contains(0) );// should be present

    // putting an unneeded element that can't fit
    BOOST_ASSERT( ! cache.put(5,1,-1) );// not added
    BOOST_ASSERT( ! cache.contains(5) );// not present
}

BOOST_AUTO_TEST_CASE( alreadyInCache )
{
    CACHE cache(1);
    cache.update(0);
    cache.put(0,1,-1);
    BOOST_CHECK_THROW( cache.put(0,1,-1), std::logic_error ); // already in cache
}

BOOST_AUTO_TEST_CASE( cacheFull )
{
    CACHE cache(10);
    cache.update(0);
    cache.update(1);
    BOOST_ASSERT( ! cache.isFull() ); // not yet full
    BOOST_ASSERT( cache.put(0,11,-1) );
    BOOST_ASSERT( cache.isFull() );// full
    BOOST_CHECK_EQUAL( cache.cacheSize(),11 );// now present
    BOOST_ASSERT( ! cache.put(1,1,1) );// can't put, we're full here
    BOOST_ASSERT( ! cache.contains(1) );// data is not here
    CACHE::data_type data;
    BOOST_ASSERT( !cache.get(1, data) );// can't get 1
}

BOOST_AUTO_TEST_CASE( fullButHigherPriorityDiscardsRequested )
{
    CACHE cache(1);

    // expected [0,1,2]
    cache.update(0);
    cache.update(1);
    cache.update(2);

    // [_,_,2]
    BOOST_ASSERT( cache.put(2,2,0) ); // pushing 2
    BOOST_ASSERT( ! cache.isFull() ); // not full, 0 is not there
    BOOST_ASSERT( cache.contains(2) );

    // [_,1,_]
    BOOST_ASSERT( cache.put(1,2,0) ); // pushing 1, removes 2
    BOOST_ASSERT( ! cache.isFull() ); // not full, 0 is not there
    BOOST_ASSERT( cache.contains(1) );
    BOOST_ASSERT( ! cache.contains(2) );

    // [0,_,_]
    BOOST_ASSERT( cache.put(0,2,0) ); // pushing 0, removes 1
    BOOST_ASSERT( cache.isFull() ); // 0 is here, and cache is full
    BOOST_ASSERT( cache.contains(0) );
    BOOST_ASSERT( ! cache.contains(1) );
}

BOOST_AUTO_TEST_CASE( discardPendings )
{
    CACHE cache(10);

    cache.update(0);
    cache.update(1);
    cache.update(3);
    // requested [0,1,3], discardable []
    //           [_,_,_]              []

    cache.put(1,2,42);
    // requested [0,1,3], discardable []
    //           [_,X,_]              []

    cache.discardPending();
    // requested [], discardable [0,1,3]
    //           []              [_,X,_]

    BOOST_CHECK_EQUAL( cache.cacheSize() ,2U);
    // jobs are not pending anymore
    BOOST_ASSERT( ! cache.isPending(0) );
    BOOST_ASSERT( ! cache.isPending(1) );
    BOOST_ASSERT( ! cache.isPending(3) );
    // but content is here
    BOOST_ASSERT( cache.contains(1) );

    CACHE::data_type data;
    BOOST_ASSERT( cache.get(1, data) );
    BOOST_CHECK_EQUAL( data, 42 );

    BOOST_CHECK_EQUAL( cache.update(1), NOT_NEEDED );
    // requested [1], discardable [0,3]
    //           [X]              [_,_]
}

BOOST_AUTO_TEST_SUITE_END()
