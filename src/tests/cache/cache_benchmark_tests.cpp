#include <dukeengine/cache/JobPriority.hpp>
#include <dukeengine/range/PlaylistRange.h>
#include <dukeengine/cache/Cache.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/thread.hpp>

#include <deque>
#include <fstream>

#define BOOST_TEST_MODULE BenchmarkTestModule
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace cache;
using namespace range;
using namespace boost::posix_time;
using namespace boost::assign;

struct JobData {
    size_t loadTime;
    size_t decodeTime;
    inline bool operator==(const JobData& other) const {
        return loadTime == other.loadTime && decodeTime == other.decodeTime;
    }
};

const JobData SENTINEL = { -1, -1 };

typedef WorkUnit<JobData, size_t, JobPriority, size_t> WORK_UNIT;

struct Job {
    Job() :
            m_JobKey(0, 0) {
    }
    Job(const deque<JobData> &data) :
            m_Data(data), m_JobKey(0, 0) {
    }

    inline void clear() {
        m_Data.clear();
    }

    inline bool empty() const {
        return m_Data.empty();
    }

    inline WORK_UNIT next() {
        WORK_UNIT wu(front());
        popFront();
        return wu;
    }
private:
    void popFront() {
        m_Data.pop_front();
        ++m_JobKey.index;
    }
    WORK_UNIT front() {
        WORK_UNIT wu(m_JobKey.index, m_JobKey);
        wu.data = m_Data.front();
        wu.metric = 1;
        return wu;
    }

    deque<JobData> m_Data;
    JobPriority m_JobKey;
};

typedef JobProducer<WORK_UNIT, Job> CACHE;

BOOST_AUTO_TEST_SUITE( BenchmarkTestSuite )

ConcurrentQueue<WORK_UNIT> decodeQueue;

inline static void sleepFor(const size_t ms) {
    boost::this_thread::sleep(boost::posix_time::millisec(ms));
}

inline static void decode(WORK_UNIT&unit) {
    sleepFor(unit.data.decodeTime);
}

inline static void load(WORK_UNIT&unit) {
    sleepFor(unit.data.loadTime);
}

inline static bool lastUnit(const WORK_UNIT&unit) {
    return unit.data == SENTINEL;
}

void worker(CACHE &jobProducer) {
    WORK_UNIT unit;
    try {
        while (true) {
            if (decodeQueue.tryPop(unit)) {
                decode(unit);
                jobProducer.push(unit);
            } else {
                jobProducer.waitAndPop(unit);
                if(lastUnit(unit)) {
                    jobProducer.terminate();
                    break;
                } else {
                    load(unit);
                    decodeQueue.push(unit);
                }
            }
        }
    } catch (chain_terminated &e) {
    }
    while(decodeQueue.tryPop(unit)) {
        decode(unit);
        jobProducer.push(unit);
    }
}

deque<JobData> loadData(const char* filename) {
    deque<JobData> data;
    ifstream file(filename);
    if(!file.is_open())
    throw std::runtime_error("unable to load file");
    while ( file.good() )
    {
        JobData entry;
        file >> entry.loadTime;
        file >> entry.decodeTime;
        data.push_back(entry);
    }
    // adding sentinel
    data.push_back(SENTINEL);
    return data;
}

const char* gchDataFilename = "src/tests/cache/data/gch.txt";
const char* nroDataFilename = "src/tests/cache/data/nro.txt";

static inline time_duration launchBench(const char *filename, const size_t threads) {
    CACHE cache(-1); // unlimited cache
    const auto data = loadData(filename);

    // launching the worker
    boost::thread_group group;
    for(size_t i=0;i<threads;++i) {
        group.create_thread(boost::bind(&worker,boost::ref(cache)));
    }

    // getting time before
    ptime start(microsec_clock::local_time());

    //pushing the job
    cache.pushJob(Job(data));

    // waiting for worker to stop
    group.join_all();
    ptime end(microsec_clock::local_time());

    BOOST_CHECK_EQUAL(cache.dump().size(), data.size()-1);
    return end-start;
}

BOOST_AUTO_TEST_CASE( benchmark )
{
    const size_t max_thread = 16;
    const vector<string> filenames = list_of(gchDataFilename)(nroDataFilename);

    typedef map<size_t, time_duration> Times;
    typedef map<string, Times > TimeMap;
    TimeMap data;
    for(const string &filename : filenames) {
        Times times;
        for( size_t i=1;i<=max_thread;++i) {
            times.insert(make_pair(i,launchBench(filename.c_str(), i)));
        }
        data.insert(make_pair(filename, times));
    }

    // analyzing the results
    for(const TimeMap::value_type &filenamePair: data) {
        cout << filenamePair.first << endl;
        time_duration reference;
        for(const Times::value_type &timePair:filenamePair.second) {
            const auto &time = timePair.second;
            cout << '#' << timePair.first<< '\t' << time.total_milliseconds() << " ms";
            if(timePair.first==1) {
                reference = time;
            } else {
                cout << "\t speedup x" << (double(reference.total_milliseconds())/time.total_milliseconds());
            }
            cout << endl;
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
