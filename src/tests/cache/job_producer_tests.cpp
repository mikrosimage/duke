#include <dukeengine/cache/JobPriority.hpp>
#include <dukeengine/cache/Cache.hpp>
#include <dukeengine/range/PlaylistRange.h>

#include <boost/thread.hpp>

#define BOOST_TEST_MODULE JobProducerTestModule
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace cache;
using namespace range;

typedef WorkUnit<string, size_t, JobPriority, size_t> WORK_UNIT;

struct Job {
    Job() :
            range(LimitedPlaylistFrameRange::EMPTY()), jobKey(-1, 0), limit(true) {
    }
    Job(const LimitedPlaylistFrameRange &range, size_t job) :
            range(range), jobKey(job, 0), limit(false) {
    }
    inline void clear() {
        limit = true;
    }
    inline bool empty() const {
        return limit || range.empty();
    }
    WORK_UNIT next() {
        WORK_UNIT wu(front());
        popFront();
        return wu;
    }
private:
    inline void popFront() {
        range.popFront();
        ++jobKey.index;
    }
    inline WORK_UNIT front() {
        assert(!empty());
        return WORK_UNIT(range.front(), jobKey);
    }
    LimitedPlaylistFrameRange range;
    JobPriority jobKey;bool limit;
};

typedef JobProducer<WORK_UNIT, Job> CACHE;

struct JobFactory {
    JobFactory(CACHE& cache) :
            m_CurrentJob(0), m_Cache(cache) {
    }

    inline void post(ptrdiff_t firstFrame, ptrdiff_t lastFrame, ptrdiff_t readFromFrame, int32_t speed) {
        m_Cache.pushJob(Job(LimitedPlaylistFrameRange(firstFrame, lastFrame, readFromFrame, speed), m_CurrentJob++));
    }
private:
    size_t m_CurrentJob;
    CACHE& m_Cache;
};

BOOST_AUTO_TEST_SUITE( JobProducerTestSuite )

BOOST_AUTO_TEST_CASE( terminatedShouldThrow )
{
    CACHE cache(-1);
    cache.terminate();

    CACHE::WorkUnit temp;
    BOOST_CHECK_THROW( cache.waitAndPop(temp), chain_terminated );
}

void popAndCheck(CACHE &cache, size_t id, size_t job, size_t index) {
    WORK_UNIT temp;
    cache.waitAndPop(temp);
    BOOST_CHECK_EQUAL( temp.id, id );
    BOOST_CHECK_EQUAL( temp.priority.jobId, job );
    BOOST_CHECK_EQUAL( temp.priority.index, index );
}

BOOST_AUTO_TEST_CASE( gettingAFewJobs )
{
    CACHE cache(-1);
    JobFactory factory(cache);

    factory.post(10,20,15,0);

    popAndCheck(cache, 15,0,0);
    popAndCheck(cache, 16,0,1);
    popAndCheck(cache, 14,0,2);
    popAndCheck(cache, 17,0,3);
    popAndCheck(cache, 13,0,4);
    popAndCheck(cache, 18,0,5);
    popAndCheck(cache, 12,0,6);
    popAndCheck(cache, 19,0,7);
    popAndCheck(cache, 11,0,8);
    popAndCheck(cache, 20,0,9);
    popAndCheck(cache, 10,0,10);

    factory.post(1,9,1,1);
    popAndCheck(cache, 1,1,0);
    popAndCheck(cache, 2,1,1);
    popAndCheck(cache, 3,1,2);
    popAndCheck(cache, 4,1,3);
    popAndCheck(cache, 5,1,4);
    popAndCheck(cache, 6,1,5);
    popAndCheck(cache, 7,1,6);
    popAndCheck(cache, 8,1,7);
    popAndCheck(cache, 9,1,8);

    factory.post(2,10,2,1);
    factory.post(5,9,5,0);
    popAndCheck(cache, 5,3,0);
    popAndCheck(cache, 6,3,1);
    popAndCheck(cache, 9,3,2);
    popAndCheck(cache, 7,3,3);
    popAndCheck(cache, 8,3,4);
}

namespace threaded_ns {
    void worker(CACHE &jobProducer, size_t stopIndex) {
        WORK_UNIT unit;
        while (true) {
            jobProducer.waitAndPop(unit);
            unit.data = "ok";
            unit.metric = 1;
            jobProducer.push(unit);
            if(unit.id == stopIndex)
            return;
        }
    }

    void getAndCheck(CACHE &cache, size_t index) {
        WORK_UNIT temp;
        BOOST_CHECK( cache.get(index,temp) );
        BOOST_CHECK( temp.data == "ok" );
    }
} // namespace threaded_ns

BOOST_AUTO_TEST_CASE( simpleThreaded )
{
    using namespace threaded_ns;

    CACHE cache(-1); // unlimited cache

    // launching the worker
    boost::thread workerThread(&worker,boost::ref(cache),10);

    // posting a job
    JobFactory factory(cache);
    factory.post(5,10,5,1);

    // waiting for worker to stop
    workerThread.join();

    // checking results
    getAndCheck(cache, 5);
    getAndCheck(cache, 6);
    getAndCheck(cache, 7);
    getAndCheck(cache, 8);
    getAndCheck(cache, 9);
    getAndCheck(cache, 10);
}

BOOST_AUTO_TEST_CASE( severalJobsWithLimit )
{
    using namespace threaded_ns;

    CACHE cache(5); // limited cache

    // launching the worker
    boost::thread workerThread(&worker,boost::ref(cache),95);

    // posting a job
    JobFactory factory(cache);
    factory.post(5,10,5,1);
    factory.post(15,20,15,1);
    factory.post(95,100,100,-1);

    // waiting for worker to stop
    workerThread.join();

    // dumping cache to analyze data
    const auto map = cache.dump();

    // we must have only 5 elements ( cache is limited to 5 )
    // the elements must be from job 2
    // the element's indices should be from 100 to 96
    BOOST_CHECK_EQUAL(map.size(), 5U );
    size_t index = 0;
    for(const CACHE::Map::value_type &pair : map) {
        BOOST_CHECK_EQUAL(pair.second.id, (100U-index) );
        BOOST_CHECK_EQUAL(pair.second.data, "ok");
        BOOST_CHECK_EQUAL(pair.first.jobId, 2U);
        BOOST_CHECK_EQUAL(pair.first.index, index);
        ++index;
    }
}

BOOST_AUTO_TEST_SUITE_END()
