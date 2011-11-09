#include <dukeengine/cache/CacheKey.hpp>
#include <dukeengine/cache/Cache.hpp>

#include <boost/assign/list_of.hpp>

#define BOOST_TEST_MODULE CacheTestModule
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace cache;

struct StringCache : public Cache<size_t, string, CacheKey> {
    virtual void limit(Map &cache) {
    }
};
typedef StringCache::WorkUnit WorkUnit;

BOOST_AUTO_TEST_SUITE( CacheTestSuite )

BOOST_AUTO_TEST_CASE( orderWorkUnit )
{
    const WorkUnit a(72,CacheKey(1,2));
    const WorkUnit b(55,CacheKey(1,0));
    const WorkUnit c(23,CacheKey(1,5));
    const WorkUnit d(56,CacheKey(0,1));

    typedef std::vector<WorkUnit> WorkUnits;
    WorkUnits jobs = boost::assign::list_of(a)(b)(c)(d);

    {
        sort(jobs.begin(), jobs.end(), &WorkUnit::keyOrdering);
        WorkUnits::const_iterator itr = jobs.begin();
        BOOST_CHECK( *itr++ == b );
        BOOST_CHECK( *itr++ == a );
        BOOST_CHECK( *itr++ == c );
        BOOST_CHECK( *itr++ == d );
        BOOST_CHECK( itr == jobs.end());
    }

    {
        sort(jobs.begin(), jobs.end(), &WorkUnit::idOrdering);
        WorkUnits::const_iterator itr = jobs.begin();
        BOOST_CHECK( *itr++ == c );
        BOOST_CHECK( *itr++ == b );
        BOOST_CHECK( *itr++ == d );
        BOOST_CHECK( *itr++ == a );
        BOOST_CHECK( itr == jobs.end());
    }
}

BOOST_AUTO_TEST_CASE( basic )
{
    StringCache cache;
    StringCache::data_type data;

    // Queue is empty
    BOOST_CHECK( !cache.get(0, data) );

    // pushing an element
    cache.manual_push(WorkUnit(5, CacheKey(0,2), "something here"));

    // Checking element in cache
    BOOST_CHECK( cache.get(5, data) );
    BOOST_CHECK_EQUAL( data, "something here" );
}

BOOST_AUTO_TEST_CASE( pushingSameIdOverridesOnlyIfNewerJob )
{
    StringCache cache;
    StringCache::data_type data;

    // pushing an element
    const StringCache::cacheid_type id(42);

    // first push will be ok
    cache.manual_push(WorkUnit(id, CacheKey(1,5), "1"));
    BOOST_CHECK( cache.get(id, data) && data == "1" );

    // this one has a higher priority (index == 2 < 5)
    // so it will be updated
    cache.manual_push(WorkUnit(id, CacheKey(1,2), "2"));
    BOOST_CHECK( cache.get(id, data) && data == "2" );

    // this one comes from an old job ( jobId == 0 < 1 )
    // it will be discarded
    cache.manual_push(WorkUnit(id, CacheKey(0,0), "3"));
    BOOST_CHECK( cache.get(id, data) && data == "2" );
}

BOOST_AUTO_TEST_CASE( limitingCache )
{
    struct MyCache : public Cache<size_t, string, CacheKey> {
        bool full;

        MyCache() : full(false) {
        }
    protected:
        virtual void limit(Map &cache) {
            cache.clear();
            full = true;
        }
    }cache;

    const MyCache::cacheid_type id(42);
    MyCache::data_type data;

    BOOST_CHECK( !cache.full );

    cache.manual_push(MyCache::WorkUnit(id, CacheKey(0,0), "something"));
    BOOST_CHECK( !cache.get(id,data) );
    BOOST_CHECK( cache.full );
}

BOOST_AUTO_TEST_SUITE_END()
