/*
 * main.cpp
 *
 *  Created on: 14 avr. 2010
 *      Author: Guillaume Chatelet
 */

#include <Chain.h>
#include <ChainOStream.h>

#include <boost/thread.hpp>
#include "boost/date_time/posix_time/posix_time.hpp" //include all types plus i/o
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#define BOOST_TEST_MODULE ChainTest
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace ::boost::this_thread;
using namespace ::boost::posix_time;

std::string hashToFilename(uint64_t hash) {
    ostringstream msg;
    msg << hash;
    return msg.str();
}

struct Worker {
private:
    vector<size_t> loadTimes;
    vector<size_t> decodeTimes;
public:
    Worker(const char* dataSetFilename) {
        loadTimes.reserve(1000);
        decodeTimes.reserve(1000);
        ifstream file(dataSetFilename);
        BOOST_ASSERT( file.is_open() );
        size_t loadTime;
        size_t decodeTime;
        while (!file.eof()) {
            file >> loadTime;
            file >> decodeTime;
            loadTimes.push_back(loadTime);
            decodeTimes.push_back(decodeTime);
        }
        assert( loadTimes.size() == decodeTimes.size());
        assert( !loadTimes.empty() );
    }

    size_t size() {
        return loadTimes.size();
    }

    void load(Chain &chain) {
        try {
            while (true) {
                const Slot::Shared shared = chain.getLoadHash();
                sleep(millisec(loadTimes[shared.m_ImageHash]));
                chain.setLoaded(shared);
            }
        } catch (chain_terminated &e) {
        }
    }

    void decode(Chain &chain) {
        try {
            while (true) {
                const Slot::Shared shared = chain.getDecodeHash();
                sleep(millisec(decodeTimes[shared.m_ImageHash]));
                chain.setDecoded(shared);
            }
        } catch (chain_terminated &e) {
        }
    }

    void loadAndDecode(Chain &chain) {
        try {
            while (true) {
                {
                    const Slot::Shared shared = chain.getLoadHash();
                    sleep(millisec(loadTimes[shared.m_ImageHash]));
                    chain.setLoaded(shared);
                }
                {
                    const Slot::Shared shared = chain.getDecodeHash();
                    sleep(millisec(decodeTimes[shared.m_ImageHash]));
                    chain.setDecoded(shared);
                }
            }
        } catch (chain_terminated &e) {
        }
    }
};

BOOST_AUTO_TEST_SUITE( ChainTestSuite )

BOOST_AUTO_TEST_CASE( ChainTransfering )
{
    TChain from;
    from.push_back(Slot(22,Slot::READY));
    from.push_back(Slot(7,Slot::DECODING));
    from.push_back(Slot(47,Slot::DECODING));
    from.push_back(Slot(9,Slot::LOADED));
    from.push_back(Slot(12,Slot::LOADING));
    from.push_back(Slot(1,Slot::NEW));

    TChain to;
    to.push_back(Slot(13));
    to.push_back(Slot(9));
    to.push_back(Slot(7));
    to.push_back(Slot(22));
    to.push_back(Slot(47));
    to.push_back(Slot(1));
    to.push_back(Slot(12));

    transferWorkUnit(from, to);

    BOOST_CHECK_EQUAL( to[0].m_Shared.m_ImageHash, 13U );
    BOOST_CHECK_EQUAL( to[0].m_State, Slot::NEW );
    BOOST_CHECK_EQUAL( to[1].m_Shared.m_ImageHash, 9U );
    BOOST_CHECK_EQUAL( to[1].m_State, Slot::LOADED );
    BOOST_CHECK_EQUAL( to[2].m_Shared.m_ImageHash, 7U );
    BOOST_CHECK_EQUAL( to[2].m_State, Slot::LOADED );
    BOOST_CHECK_EQUAL( to[3].m_Shared.m_ImageHash, 22U );
    BOOST_CHECK_EQUAL( to[3].m_State, Slot::READY );
    BOOST_CHECK_EQUAL( to[4].m_Shared.m_ImageHash, 47U );
    BOOST_CHECK_EQUAL( to[4].m_State, Slot::LOADED );
    BOOST_CHECK_EQUAL( to[5].m_Shared.m_ImageHash, 1U );
    BOOST_CHECK_EQUAL( to[5].m_State, Slot::NEW );
    BOOST_CHECK_EQUAL( to[6].m_Shared.m_ImageHash, 12U );
    BOOST_CHECK_EQUAL( to[6].m_State, Slot::NEW );
}

BOOST_AUTO_TEST_CASE( ChainFindFirst )
{
    TChain chain;
    chain.push_back(Slot(100,Slot::NEW));
    chain.push_back(Slot(1,Slot::LOADING));
    chain.push_back(Slot(2,Slot::LOADED));
    chain.push_back(Slot(3,Slot::DECODING));
    chain.push_back(Slot(4,Slot::READY));

    std::size_t accelerator=0;
    BOOST_CHECK_EQUAL( chain.quickFind(Slot::NEW, accelerator)->m_Shared.m_ImageHash, 100U );
    BOOST_CHECK_EQUAL( accelerator, 1U );

    accelerator=0;
    BOOST_CHECK_EQUAL( chain.quickFind(Slot::LOADING, accelerator)->m_Shared.m_ImageHash, 1U );
    BOOST_CHECK_EQUAL( accelerator, 2U );

    accelerator=0;
    BOOST_CHECK_EQUAL( chain.quickFind(Slot::LOADED, accelerator)->m_Shared.m_ImageHash, 2U );
    BOOST_CHECK_EQUAL( accelerator, 3U );

    accelerator=0;
    BOOST_CHECK_EQUAL( chain.quickFind(Slot::DECODING, accelerator)->m_Shared.m_ImageHash, 3U );
    BOOST_CHECK_EQUAL( accelerator, 4U );

    accelerator=0;
    BOOST_CHECK_EQUAL( chain.quickFind(Slot::READY, accelerator)->m_Shared.m_ImageHash, 4U );
    BOOST_CHECK_EQUAL( accelerator, 5U );

    accelerator=0;
    chain.pop_back();
    BOOST_CHECK( chain.quickFind(Slot::READY, accelerator) == chain.end() );
}

BOOST_AUTO_TEST_CASE( hasDoubleCheck )
{
    SimpleIndexRange<> noDouble(0,2);
    BOOST_CHECK( hasDouble(noDouble) == false );
    ConstantIndexRange<> stillNoDouble(0,1);
    BOOST_CHECK( hasDouble(stillNoDouble) == false );
    ConstantIndexRange<> doubles(0,2);
    BOOST_CHECK( hasDouble(doubles) );
}

BOOST_AUTO_TEST_CASE( getFilenameFromHash )
{
    SimpleIndexRange<uint64_t> iterator(100,101);
    Chain chain;
    chain.postNewJob(iterator, &::hashToFilename);
    std::string filename;
    chain.getFilenameForHash(100, filename);
    BOOST_CHECK_EQUAL( "100", filename );
}

BOOST_AUTO_TEST_CASE( ChainGetIndex )
{
    SimpleIndexRange<uint64_t> iterator(100,101);
    Chain chain;
    chain.postNewJob(iterator, &::hashToFilename);

    Slot::Shared shared = chain.getLoadHash();
    BOOST_CHECK_EQUAL( shared.m_ImageHash, 100U);
    shared.m_ImageHash = 0;
    chain.setLoaded(shared); // ok to give an unneeded data
    shared.m_ImageHash = 100;
    chain.setLoaded(shared); // provisioning slot
    shared.m_ImageHash = 0;
    chain.setDecoded(shared); // ok to give an unneeded data
    shared.m_ImageHash = 100;
    chain.setDecoded(shared); // provisioning slot
    Slot::Shared slot;
    BOOST_CHECK_EQUAL( chain.getResult(100, slot), true );

    // not allowing getting frame not in the current range
    BOOST_CHECK_THROW( chain.getResult(0, slot), std::runtime_error );
}

boost::posix_time::time_duration bench(const size_t decoderCount, Worker& worker) {
    assert(decoderCount>=0 && decoderCount<10);
    Chain chain;
    Chain::WorkerThreadFunctions functions;
    if(decoderCount==0)
    functions.push_back(bind(&Worker::loadAndDecode, &worker, _1));
    else
    functions.push_back(bind(&Worker::load, &worker, _1));
    for(unsigned i=0;i<decoderCount;++i)
    functions.push_back(bind(&Worker::decode, &worker, _1));

    return chain.benchmark(functions, &::hashToFilename, worker.size());
}

void launchTestSet(const char* filename) {
    Worker worker(filename);
    cout << "reading " << worker.size() << " virtual images from " << filename << endl;
    cout << "nothread              : " << bench(0, worker) << endl;
    for(size_t i=1;i<8;++i)
    cout << "one loader, "<<i<<" decoder : " << bench(i, worker) << endl;
}

BOOST_AUTO_TEST_CASE( Benchmark )
{
//    launchTestSet("data/nro.txt");
//    launchTestSet("data/gch.txt");
}

BOOST_AUTO_TEST_SUITE_END()
