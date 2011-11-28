#include <ConcurrentQueue.h>
#include <LookAheadCache.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/thread.hpp>

#include <deque>
#include <fstream>

#define BOOST_TEST_MODULE BenchmarkTestModule
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace cache;
using namespace boost::posix_time;
using namespace boost::assign;

struct JobData {
    size_t loadTime;
    size_t decodeTime;

    inline bool operator<(const JobData& other) const {
        if (loadTime == other.loadTime)
            return decodeTime < other.decodeTime;
        return loadTime < other.loadTime;
    }
};

typedef JobData* id_type;

struct Job {
    Job() {
    }

    Job(const deque<JobData> &data) :
            m_Data(data), m_Index(0) {
    }

    inline void clear() {
        m_Index = m_Data.size();
    }

    inline bool empty() const {
        return m_Index >= m_Data.size();
    }

    inline id_type next() {
        return &m_Data[m_Index++];
    }

private:
    deque<JobData> m_Data;
    size_t m_Index;
};

typedef size_t metric_type;
typedef size_t data_type;
typedef LookAheadCache<id_type, metric_type, data_type, Job> CACHE;

BOOST_AUTO_TEST_SUITE( BenchmarkTestSuite )

ConcurrentQueue<id_type> decodeQueue;

inline static void sleepFor(const size_t ms) {
    boost::this_thread::sleep(boost::posix_time::millisec(ms));
}

inline static void decode(const JobData &unit) {
    sleepFor(unit.decodeTime);
}

inline static void load(const JobData &unit) {
    sleepFor(unit.loadTime);
}

inline static bool lastUnit(const JobData &unit) {
    return unit.loadTime == size_t(-1) && unit.decodeTime==size_t(-1);
}

void worker(CACHE &jobProducer) {
    JobData *pUnit = NULL;
    try {
        while (true) {
            if (decodeQueue.tryPop(pUnit)) {
                decode(*pUnit);
                jobProducer.offer(pUnit, 1, 0);
            } else {
                jobProducer.waitAndPop(pUnit);
                if(lastUnit(*pUnit)) {
                    jobProducer.terminate();
                    break;
                } else {
                    load(*pUnit);
                    decodeQueue.push(pUnit);
                }
            }
        }
    } catch (terminated &e) {
    }
    while(decodeQueue.tryPop(pUnit)) {
        decode(*pUnit);
        jobProducer.offer(pUnit, 1, 0);
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
    data.push_back(JobData {-1,-1});
    return data;
}

const char* gchDataFilename = "tests/data/gch.txt";
const char* nroDataFilename = "tests/data/nro.txt";

static inline time_duration launchBench(const char *filename, const size_t threads) {
    CACHE cache(-1); // unlimited cache
    const auto data = loadData(filename);

    // launching the worker
    boost::thread_group group;
    for(size_t i=0;i<threads;++i)
    group.create_thread(boost::bind(&worker,boost::ref(cache)));

    // getting time before
    ptime start(microsec_clock::local_time());

    //pushing the job
    cache.pushJob(Job(data));

    // waiting for worker to stop
    group.join_all();
    ptime end(microsec_clock::local_time());

//    BOOST_CHECK_EQUAL(cache.dump().size(), data.size()-1);
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
