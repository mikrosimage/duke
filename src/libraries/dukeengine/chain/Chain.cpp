/*
 * Chain.cpp
 *
 *  Created on: 10 nov. 2010
 *      Author: Guillaume Chatelet
 */

#include "Chain.h"

#include <iterator>
#define __STDC_LIMIT_MACROS
#include <stdint.h>

using namespace std;

SlotData::~SlotData() {
}

Slot::Slot() :
    m_Shared(0), m_State(NEW) {
}
Slot::Slot(uint64_t imageHash) :
    m_Shared(imageHash), m_State(NEW) {
}
Slot::Slot(uint64_t imageHash, State state) :
    m_Shared(imageHash), m_State(state) {
}
bool Slot::operator<(const Slot& other) const {
    return m_Shared.m_ImageHash < other.m_Shared.m_ImageHash;
}

TChain::TChain() {
    reserve(InitialChainReserve);
}
TChain::TChain(OnePassRange<uint64_t> &iterator) {
    reserve(InitialChainReserve);
    for (; !iterator.empty(); iterator.popFront())
        push_back(Slot(iterator.front()));
}
TChain::iterator TChain::quickFind(const Slot::State state, size_t &fromIndex) {
    iterator itr = begin();
    advance(itr, fromIndex);
    const iterator end = this->end();
    for (; itr != end; ++itr) {
        ++fromIndex;
        if (itr->m_State == state)
            return itr;
    }
    fromIndex = 0;
    return end;
}
TChain::iterator TChain::find(const uint64_t imageHash) {
    const iterator end = this->end();
    for (iterator itr = begin(); itr != end; ++itr)
        if (itr->m_Shared.m_ImageHash == imageHash)
            return itr;
    return end;
}
TChain::const_iterator TChain::safeFind(const uint64_t imageHash) const {
    const const_iterator end = this->end();
    for (const_iterator itr = begin(); itr != end; ++itr)
        if (itr->m_Shared.m_ImageHash == imageHash)
            return itr;
    throw std::runtime_error("safeFind forbids asking for a frame not in the range");
}

void transferWorkUnit(TChain &from, TChain &to) {
    sort(from.begin(), from.end());
    const TChain::const_iterator sortedFrom_Begin = from.begin();
    const TChain::const_iterator sortedFrom_End = from.end();
    for (TChain::iterator destinationItr = to.begin(); destinationItr != to.end(); ++destinationItr) {
        Slot &current = *destinationItr; // the current element
        const TChain::const_iterator fromSlotItr = lower_bound(sortedFrom_Begin, sortedFrom_End, current);
        if (fromSlotItr == sortedFrom_End || fromSlotItr->m_Shared.m_ImageHash != current.m_Shared.m_ImageHash || fromSlotItr->m_State == Slot::NEW)
            continue;
        current = *fromSlotItr;
        switch (current.m_State) {
            case Slot::DECODING:
                current.m_State = Slot::LOADED;
                break;
            case Slot::LOADING:
                current.m_State = Slot::NEW;
                break;
            case Slot::NEW:
            case Slot::READY:
            case Slot::LOADED:
                break;
        }
    }
}

ChainContext::~ChainContext() {
}

Chain::Chain() :
    m_Terminate(false) {
    // we want the Slot to be fast to iterate over
    assert( sizeof(Slot)==32 );
    cleanAccelerators();
}
Chain::~Chain() {
    stopWorkers();
}

void Chain::stopWorkers() {
    if (m_ThreadGroup.size() == 0)
        return; // no thread to stop
    {
        boost::mutex::scoped_lock lock(m_ChainMutex);
        m_Terminate = true;
    }
    m_condNewJob.notify_all();
    m_condSlotLoaded.notify_all();
    m_condSlotDecoded.notify_all();
    m_ThreadGroup.join_all();
}

void Chain::postNewJob(ForwardRange<uint64_t> &iterator, const HashToFilenameFunction &function) {
    assert( !hasDouble(iterator) ); // ensuring no doubles in list
    TChain newJobChain(iterator);
    {// locking the chain for reordering
        boost::mutex::scoped_lock lock(m_ChainMutex);
        {// locking the loadFunction for write
            boost::unique_lock<boost::shared_mutex> writeLock(m_LoadFunctionReadWriteMutex);
            m_LoadFunction = function;
        }
        // moving previous work from old chain to new chain
        transferWorkUnit(m_Chain, newJobChain);
        // now working on the new chain
        m_Chain.swap(newJobChain);
        cleanAccelerators();
    }
    m_condNewJob.notify_one();
}

boost::posix_time::time_duration Chain::benchmark(const WorkerThreadFunctions &functions, const HashToFilenameFunction &function, const size_t count) {
//    assert(count<SIZE_MAX);
    using namespace boost::posix_time;
    using namespace boost;
    SimpleIndexRange<uint64_t> iterator(0, count + 1);
    addWorker(functions);
    ptime start = microsec_clock::local_time();
    postNewJob(iterator, function);
    Slot::Shared slot;
    for (size_t i = 0; i <= count; ++i)
        getResult(i, slot);
    stopWorkers();
    return microsec_clock::local_time() - start;
}

Slot::Shared Chain::getHash(const Slot::State stateToFind, const Slot::State stateToSet, boost::condition &conditionToCheck) {
    assert(stateToFind==Slot::NEW || stateToFind==Slot::LOADED);
    TChain::iterator pSlot;
    boost::mutex::scoped_lock lock(m_ChainMutex);
    if (m_Terminate)
        throw chain_terminated();
    const size_t acceleratorIndex = stateToFind == Slot::NEW ? 0 : 1;
    while (m_Chain.empty() || (pSlot = m_Chain.quickFind(stateToFind, m_LastIndexAccelerator[acceleratorIndex])) == m_Chain.end()) {
        conditionToCheck.wait(lock);
        if (m_Terminate)
            throw chain_terminated();
    }
    pSlot->m_State = stateToSet;
    return pSlot->m_Shared;
}

void Chain::setData(const Slot::Shared &shared, const Slot::State stateToSet, boost::condition &conditionToNotify) {
    boost::mutex::scoped_lock lock(m_ChainMutex);
    const TChain::iterator pSlot = m_Chain.find(shared.m_ImageHash);
    // the slot we are provisioning might not be needed anymore
    if (pSlot == m_Chain.end())
        return;
    pSlot->m_State = stateToSet;
    pSlot->m_Shared = shared;
    lock.unlock();
    conditionToNotify.notify_one();
}

bool Chain::getResult(const uint64_t &imageHash, Slot::Shared &ptr) const {
    TChain::const_iterator pSlot;
    boost::mutex::scoped_lock lock(m_ChainMutex);
    if (m_Terminate)
        return false;
    // safeFind will throw if imageIndex is not in the job's range
    // so dereferencing pSlot is always valid
    while ((pSlot = m_Chain.safeFind(imageHash))->m_State != Slot::READY) {
        m_condSlotDecoded.wait(lock);
        if (m_Terminate)
            return false;
    }
    ptr = pSlot->m_Shared;
    return true;
}

void Chain::getFilenameForHash(const uint64_t &hash, std::string &filename) const {
    // locking the loadFunction for read
    boost::shared_lock<boost::shared_mutex> readLock(m_LoadFunctionReadWriteMutex);
    filename = m_LoadFunction(hash);
}

void Chain::addWorker(const WorkerThreadFunction &function) {
    m_ThreadGroup.add_thread(new boost::thread(function, boost::ref(*this)));
}

void Chain::addWorker(const std::vector<WorkerThreadFunction> &functions) {
    typedef std::vector<WorkerThreadFunction> TVector;
    for (TVector::const_iterator itr = functions.begin(); itr != functions.end(); ++itr)
        addWorker(*itr);
}
