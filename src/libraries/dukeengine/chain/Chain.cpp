#include "Chain.h"

#include <iterator>
#define __STDC_LIMIT_MACROS
#include <stdint.h>

using namespace std;

SlotData::~SlotData() {
}

InternalSlot::InternalSlot() :
    m_State(NEW), m_Shared(0) {
}
InternalSlot::InternalSlot(uint64_t imageHash) :
    m_State(NEW), m_Shared(imageHash) {
}
InternalSlot::InternalSlot(uint64_t imageHash, State state) :
    m_State(state), m_Shared(imageHash) {
}
bool InternalSlot::operator<(const InternalSlot& other) const {
    return m_Shared.m_ImageHash < other.m_Shared.m_ImageHash;
}

TChain::TChain() {
    reserve(InitialChainReserve);
}

TChain::TChain(OnePassRange<uint64_t> &iterator) {
    reserve(InitialChainReserve);
    for (; !iterator.empty(); iterator.popFront())
        push_back(InternalSlot(iterator.front()));
}

TChain::iterator TChain::quickFind(const State state, size_t &fromIndex) {
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

template<typename ITR>
inline ITR TChain::find(ITR itr, const ITR end, const uint64_t imageHash) {
    for (; itr != end; ++itr)
        if (itr->m_Shared.m_ImageHash == imageHash)
            return itr;
    return end;
}

TChain::iterator TChain::find(const uint64_t imageHash) {
    return find<iterator> (begin(), end(), imageHash);
}

TChain::const_iterator TChain::find(const uint64_t imageHash) const {
    return find<const_iterator> (begin(), end(), imageHash);
}

TChain::const_iterator TChain::safeFind(const uint64_t imageHash) const {
    const const_iterator end = this->end();
    const_iterator itr = find<const_iterator> (begin(), end, imageHash);
    if (itr == end)
        throw std::runtime_error("safeFind forbids asking for a frame not in the range ");
    return itr;
}

Chain::Chain() :
    m_Terminate(false) {
    // we want the Slot to be fast to iterate over
    assert( sizeof(InternalSlot)==32 );
    resetAccelerators();
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

/**
 * this function will walk through the new job and try to reuse the
 * slots from the previous job.
 *
 */
void transferWorkUnit(TChain &from, TChain &to) {
    sort(from.begin(), from.end());
    const TChain::const_iterator sortedFrom_Begin = from.begin();
    const TChain::const_iterator sortedFrom_End = from.end();
    for (TChain::iterator destinationItr = to.begin(); destinationItr != to.end(); ++destinationItr) {
        InternalSlot &current = *destinationItr; // the current element
        const TChain::const_iterator fromSlotItr = lower_bound(sortedFrom_Begin, sortedFrom_End, current);
        if (fromSlotItr == sortedFrom_End // element not found
                || fromSlotItr->m_Shared.m_ImageHash != current.m_Shared.m_ImageHash // not same hash
                || fromSlotItr->m_State == NEW) //element not filled
            continue;
        current = *fromSlotItr;
        switch (current.m_State) {
            case DECODING:
                current.m_State = LOADED;
                break;
            case LOADING:
                current.m_State = NEW;
                break;
            case NEW:
            case READY:
            case LOADED:
            case UNDEFINED:
                break;
        }
    }
}

void Chain::postNewJob(ForwardRange<uint64_t> &iterator, const HashToFilenameFunction &function) {

    assert( !hasDouble(iterator) ); // ensuring no index doubles
    TChain newJobChain(iterator);
    {// locking the chain for reordering
        boost::mutex::scoped_lock lock(m_ChainMutex);
        {// locking the loadFunction for write and releasing ASAP
            boost::unique_lock<boost::shared_mutex> writeLock(m_LoadFunctionReadWriteMutex);
            m_LoadFunction = function;
        }

        // moving previous work from old chain to new chain
        transferWorkUnit(m_Chain, newJobChain);

        // now working on the new chain
        m_Chain.swap(newJobChain);

        // evicting unneeded elements
        shrinkChain();

        // reseting the chain accelerators
        resetAccelerators();
    }
    m_condNewJob.notify_one();
}

boost::posix_time::time_duration Chain::benchmark(const WorkerThreadFunctions &functions, const HashToFilenameFunction &function, const size_t count) {
    assert(count<SIZE_MAX);
    using namespace boost::posix_time;
    using namespace boost;
    SimpleIndexRange<uint64_t> iterator(0, count + 1);
    addWorker(functions);
    ptime start = microsec_clock::local_time();
    postNewJob(iterator, function);
    Slot slot;
    for (size_t i = 0; i <= count; ++i)
        getResult(i, slot);
    stopWorkers();
    return microsec_clock::local_time() - start;
}

Slot Chain::getHash(const State stateToFind, const State stateToSet, boost::condition &conditionToCheck) {
    assert(stateToFind==NEW || stateToFind==LOADED);
    TChain::iterator pSlot;
    boost::mutex::scoped_lock lock(m_ChainMutex);
    if (m_Terminate)
        throw chain_terminated();
    const size_t acceleratorIndex = stateToFind == NEW ? 0 : 1;
    while (m_Chain.empty() || (pSlot = m_Chain.quickFind(stateToFind, m_LastIndexAccelerator[acceleratorIndex])) == m_Chain.end()) {
        conditionToCheck.timed_wait(lock, boost::posix_time::millisec(10));
        if (m_Terminate)
            throw chain_terminated();
    }
    pSlot->m_State = stateToSet;
    return pSlot->m_Shared;
}

bool Chain::setData(const Slot &shared, const State stateToSet, boost::condition &conditionToNotify) {
    boost::mutex::scoped_lock lock(m_ChainMutex);
    const TChain::iterator pSlot = m_Chain.find(shared.m_ImageHash);
    // the slot we are provisioning might not be needed anymore
    if (pSlot == m_Chain.end())
        return false;
    // adding the slot
    pSlot->m_State = stateToSet;
    pSlot->m_Shared = shared;

    // shrinking the chain to maintain resources
    shrinkChain();

    // unlocking
    lock.unlock();
    conditionToNotify.notify_one();
    return true;
}

bool Chain::getResult(const uint64_t &imageHash, Slot &ptr) const {
    TChain::const_iterator pSlot;
    boost::mutex::scoped_lock lock(m_ChainMutex);
    if (m_Terminate)
        return false;
    // safeFind will throw if imageIndex is not in the job's range
    // so dereferencing pSlot is always valid
    while ((pSlot = m_Chain.safeFind(imageHash))->m_State != READY) {
        m_condSlotDecoded.wait(lock);
        if (m_Terminate)
            return false;
    }
    ptr = pSlot->m_Shared;
    return true;
}

void Chain::dump(ForwardRange<uint64_t> & range, const uint64_t imageHash) const {
    boost::mutex::scoped_lock lock(m_ChainMutex);
    if (m_Terminate)
        return;

    assert(NEW == 0 && LOADING == 1&&LOADED == 2&& DECODING == 3&& READY == 4&& UNDEFINED == 5);
    const char stateChar[][2] = { { 'n', 'N' }, { 'n', 'N' }, { 'l', 'L' }, { 'l', 'L' }, { 'r', 'R' }, { 'x', 'X' } };
    const TChain::const_iterator end = m_Chain.end();

    std::stringstream ssdump;
    ssdump << '[';

    for (TChain::const_iterator pSlot; !range.empty(); range.popFront()) {
        const uint64_t index = range.front();
        pSlot = m_Chain.find(index);
        const size_t stateIndex = pSlot == end ? UNDEFINED : pSlot->m_State;
        const size_t isCursorIndex = imageHash == index ? 1 : 0;
        ssdump << stateChar[stateIndex][isCursorIndex];
    }
    ssdump << "]\r";

    cout << ssdump.str();
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
