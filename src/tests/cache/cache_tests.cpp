#include <dukeengine/cache/JobPriority.hpp>
#include <dukeengine/cache/Cache.hpp>

#define BOOST_TEST_MODULE CacheTestModule
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace cache;

typedef WorkUnit<bool, size_t, JobPriority, size_t> WORK_UNIT;
typedef Cache<WORK_UNIT> CACHE;

BOOST_AUTO_TEST_SUITE( CacheTestSuite )

/**
 * WorkUnit tests
 */

BOOST_AUTO_TEST_CASE( workUnit )
{
    const WORK_UNIT wu(0,JobPriority(1,2));
    BOOST_CHECK_EQUAL( wu.id, 0U);
    BOOST_CHECK_EQUAL( wu.priority.jobId, 1U);
    BOOST_CHECK_EQUAL( wu.priority.index, 2U);
    BOOST_CHECK_EQUAL( wu.metric, 0U);
}

/**
 * Testing get and push on cache
 */

BOOST_AUTO_TEST_CASE( basicCache )
{
    CACHE cache(-1);
    WORK_UNIT wu;
    BOOST_CHECK( ! cache.get(0,wu) );

    wu.id = 0;
    wu.priority=JobPriority(0,0);
    wu.metric=1;
    BOOST_CHECK( cache.push(wu) );

    BOOST_CHECK( cache.get(0,wu) );
    BOOST_CHECK_EQUAL( wu.id, 0U);
    BOOST_CHECK_EQUAL( wu.priority.jobId, 0U);
    BOOST_CHECK_EQUAL( wu.priority.index, 0U);
    BOOST_CHECK_EQUAL( wu.metric, 1U);
}

BOOST_AUTO_TEST_CASE( unitDoesntFit )
{
    CACHE cache(0); // max cache size = 0
    WORK_UNIT wu(0,JobPriority(0,0),1);
    BOOST_CHECK( ! cache.push(wu) );// unit of size 1 wont fit
}

BOOST_AUTO_TEST_CASE( secondUnitDoesntFit )
{
    CACHE cache(1);
    BOOST_CHECK( cache.push(WORK_UNIT(0,JobPriority(0,0),1)) ); // first ok
    BOOST_CHECK( ! cache.push(WORK_UNIT(1,JobPriority(0,1),1)) );// second can't fit

    WORK_UNIT wu;
    BOOST_CHECK( cache.get(0,wu) );
    BOOST_CHECK( ! cache.get(1,wu) );
}

BOOST_AUTO_TEST_CASE( higherPriority )
{
    CACHE cache(1);
    BOOST_CHECK( cache.push(WORK_UNIT(0,JobPriority(0,0),1)) );
    BOOST_CHECK( cache.push(WORK_UNIT(1,JobPriority(1,0),1)) );

    WORK_UNIT wu;
    BOOST_CHECK( ! cache.get(0,wu) ); // index 0 was replaced
    BOOST_CHECK( cache.get(1,wu) );// by index 1 with higher priority
}

BOOST_AUTO_TEST_CASE( lowerPriorityButStillSomeSpaceLeft )
{
    CACHE cache(2);
    BOOST_CHECK( cache.push(WORK_UNIT(0,JobPriority(0,0),1)) );
    BOOST_CHECK( cache.push(WORK_UNIT(1,JobPriority(0,1),1)) );

    WORK_UNIT wu;
    BOOST_CHECK( cache.get(0,wu) );
    BOOST_CHECK( cache.get(1,wu) );
}

/**
 * Testing query
 */
BOOST_AUTO_TEST_CASE( updatePriorityNotInCache )
{
    CACHE cache(2);
    const size_t id = 0;
    const WORK_UNIT wu(id,JobPriority(0,0),1);
    BOOST_CHECK_EQUAL( cache.query(wu) , REQUIRED);

    // now putting in the cache
    BOOST_CHECK( cache.push(wu) );

    // updating with higher priority
    BOOST_CHECK_EQUAL( cache.query(WORK_UNIT(id,JobPriority(1,0),5)) , DONE);

    WORK_UNIT temp;
    BOOST_CHECK( cache.get(id,temp) );
    BOOST_CHECK_EQUAL( temp.priority.jobId, 1U);// priority should be updated
    BOOST_CHECK_EQUAL( temp.metric, 1U);// metric should  NOT be updated

    // updating with lowest priority
    const WORK_UNIT lowestPriority(id,JobPriority(0,0),1);
    BOOST_CHECK_EQUAL( cache.query(lowestPriority) , DONE);

    BOOST_CHECK( cache.get(id,temp) );
    BOOST_CHECK_EQUAL( temp.priority.jobId, 1U);// priority should NOT be updated
    BOOST_CHECK_EQUAL( temp.metric, 1U);// metric should NOT be updated

    // making cache full
    BOOST_CHECK( cache.push(WORK_UNIT(1,JobPriority(1,1),1)) );

    // asking for a new item
    BOOST_CHECK_EQUAL( cache.query(WORK_UNIT(2,JobPriority(1,2),1)) , CACHE_FULL);
}

BOOST_AUTO_TEST_CASE( test ){
    WorkUnit<bool, size_t, JobPriority, uint64_t> wu;
}

BOOST_AUTO_TEST_SUITE_END()
