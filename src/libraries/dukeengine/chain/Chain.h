#ifndef CHAIN_H_
#define CHAIN_H_

#include "RangeImpl.h"

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/shared_ptr.hpp>

#include "boost/date_time/posix_time/posix_time_types.hpp" // no io to keep it "light"
#include <iostream>
#include <vector>

struct chain_terminated : public std::runtime_error {
    explicit chain_terminated() :
        std::runtime_error("chain is terminated and won't subsequent incoming call") {
    }
};

/**
 * An interface you can inherit to store whatever data you want in a Slot.
 * The chain knows nothing about what's done during load and decode
 */
struct SlotData {
    virtual ~SlotData()=0;
};

/**
 * The Slot.
 * It contains the image hash ( id ) and some private data in the form of a smart pointer.
 */
struct Slot {
    typedef boost::shared_ptr<SlotData> TSlotDataPtr;
    /**
     * we cannot use a simple playlist index as an image identifier
     * because if playlist changes we have no way to say : "this is the same image slot"
     * and we would lost all the previous loaded slot.
     * So we are using an image identifier in the form of an uint64_t hash
     * split in two uint_32t
     * - hash of the path+filename+extension computed once per clip
     * - image index in the sequence
     * this will guaranty no collision and fast computation
     */
    uint64_t m_ImageHash;
    TSlotDataPtr m_pSlotData;
    Slot() :
        m_ImageHash(0) {
    }
    Slot(uint64_t hash) :
        m_ImageHash(hash) {
    }
    Slot(uint64_t hash, const TSlotDataPtr &ptr) :
        m_ImageHash(hash), m_pSlotData(ptr) {
    }
};

/**
 * The slot state.
 */
enum State {
    NEW = 0, LOADING = 1, LOADED = 2, DECODING = 3, READY = 4, UNDEFINED = 5
};

/**
 * An internal structure needed to store the Slot state.
 * This state is managed by the Chain and should not be
 * accessed otherwise.
 */
struct InternalSlot {
    State m_State;
    Slot m_Shared;

    InternalSlot();
    InternalSlot(uint64_t imageHash);
    InternalSlot(uint64_t imageHash, State state);
    bool operator<(const InternalSlot& other) const;

    // ensuring the struct is aligned to maximize cache hit
}__attribute__((aligned (32)));

/**
 * The chain container. Basically a POD (Plain Old Data) structure
 * with a few helper functions
 */
struct TChain : public std::vector<InternalSlot> {
    const static size_t InitialChainReserve = 200;
    TChain();
    TChain(OnePassRange<uint64_t> &iterator);
    /**
     * Quickly finds a slot with the specified state from the specified
     * index.
     */
    iterator quickFind(const State state, size_t &fromIndex);
    /**
     * Finds the slot with the specified hash
     */
    iterator find(const uint64_t);
    /**
     * Finds the slot with the specified hash
     */
    const_iterator find(const uint64_t) const;
    /**
     * Finds the slot with the specified hash, throws if not found
     */
    const_iterator safeFind(const uint64_t) const;
private:
    template<typename ITR>
    static inline ITR find(ITR begin, const ITR end, const uint64_t);
};

/**
 * When a new job is posted we don't want to trash the already
 * loaded and decoded slots. This function will be in charge of moving those
 * slots from the old chain to the new one.
 */
void transferWorkUnit(TChain &from, TChain &to);

/**
 * Iterates the range and checks if it contains any double
 * This is just a check and might be omitted in production
 */
#include <set>
template<typename T>
inline bool hasDouble(const ForwardRange<T>&iterator) {
    std::auto_ptr<ForwardRange<T> > pCopy(iterator.save());
    std::set<T> aSet;
    std::size_t count = 0;
    for (; !pCopy->empty(); pCopy->popFront(), ++count)
        aSet.insert(pCopy->front());
    return aSet.size() != count;
}

/**
 * The Chain interface
 */
class Chain : boost::noncopyable {
public:
    typedef boost::function<std::string(uint64_t)> HashToFilenameFunction;
    typedef boost::function<void(Chain&)> WorkerThreadFunction;
    typedef std::vector<WorkerThreadFunction> WorkerThreadFunctions;
private:
    /**
     * Variables marked with '///< shared' are accessed across
     * several threads and need special care to prevent data races.
     */
    bool m_Terminate; ///< shared
    // The main structure gathering all the data
    TChain m_Chain; ///< shared
    // All the stuff to protect it and allow minimal synchronization
    mutable boost::mutex m_ChainMutex;
    mutable boost::condition m_condNewJob;
    mutable boost::condition m_condSlotLoaded;
    mutable boost::condition m_condSlotDecoded;
    // accelerators so we wont search for index from the M_Chain.begin() every time
    std::size_t m_LastIndexAccelerator[2]; ///< shared

    mutable boost::shared_mutex m_LoadFunctionReadWriteMutex;
    HashToFilenameFunction m_LoadFunction; ///< shared
    boost::thread_group m_ThreadGroup;

    Slot getHash(const State, const State, boost::condition&);
    bool setData(const Slot &, const State, boost::condition&);
    void stopWorkers();
    inline void resetAccelerators() {
        m_LastIndexAccelerator[0] = 0;
        m_LastIndexAccelerator[1] = 0;
    }
    inline bool shrinkChain() {
        const size_t newSize = getNewEndIndex(m_Chain);
        if (newSize < m_Chain.size()) {
            m_Chain.resize(newSize);
            resetAccelerators();
            return true;
        }
        return false;
    }
protected:
    /**
     * This method is used to restrict resource usage within the Chain.
     * This method will be called by the chain when elements are inserted or updated.
     * The return value will be used to evict elements from the chain and keep
     * the resources low. Default policy is to evict nothing.
     *
     * /!\ This function must be reentrant, it will be called from many threads
     * at the same time.
     */
    virtual size_t getNewEndIndex(const TChain& chain) const {
        return chain.size();
    }
public:
    Chain();
    virtual ~Chain();
    /**
     * Post a new job in the form of an index range.
     * This range must be bounded.
     *
     * The range is iterated over to provided indices.
     * Indices are actually hash that uniquely identifies an image to load.
     *
     * The worker threads can call the getFilenameForHash() function to
     * retrieve the filename associated with the hash. See below.
     *
     * /!\ !!! range must not contain twice the same index !!!
     * This precondition is checked in debug mode
     */
    void postNewJob(ForwardRange<uint64_t> &range, const HashToFilenameFunction &function);

    /**
     * append workers to this Chain to multithread load and decode
     */
    void addWorker(const WorkerThreadFunction &function);
    void addWorker(const WorkerThreadFunctions &functions);

    // returns the time needed to decode 'count' images with 'functions' instantiated as workers
    boost::posix_time::time_duration benchmark(const WorkerThreadFunctions &functions, const HashToFilenameFunction &function, const size_t count);

    /**
     * Worker threads will call the following functions to fill the chain.
     *
     * Those functions are thread safe. A Slot object is copied back
     * and forth to keep locking as low as possible. Slot object is very
     * lightweight and copy will be cheap ( sizeof(Slot)==32 ).
     *
     * Those functions will block if no more work is available preventing
     * threads from consuming system resources.
     */
    inline Slot getLoadSlot() {
        return getHash(NEW, LOADING, m_condNewJob);
    }
    inline Slot getDecodeSlot() {
        return getHash(LOADED, DECODING, m_condSlotLoaded);
    }
    // returns true if the slot was actually added, false if not needed any more
    inline bool setLoadedSlot(Slot slot) {
        return setData(slot, LOADED, m_condSlotLoaded);
    }
    // returns true if the slot was actually added, false if not needed any more
    inline bool setDecodedSlot(Slot slot) {
        return setData(slot, READY, m_condSlotDecoded);
    }
    /**
     * This function is synchronized ( multiple readers / one writer )
     * and is used to retrieve the filename corresponding to the hash.
     *
     * Delaying the creation of the filename to the very last moment
     * will keep the Slot light and prevent unnecessary computation.
     *
     * NB : A worker might still be loading an image from a previous job.
     * If this slot is not needed for the new job it will be gently discarded.
     * otherwise it will be inserted in the chain at the updated position.
     */
    void getFilenameForHash(const uint64_t &hash, std::string &filename) const;

    /**
     * Client function allowing to fetch already loaded image from the cache.
     *
     * - returns true if slot is in READY state
     * - returns false if no worker could fulfill the job at that time
     * - throws if asking for an image outside of the current job's range
     */
    bool getResult(const uint64_t &imageHash, Slot &slot) const;

    /**
     * /!\ this is synchronized and will degrade performance seriously
     * use only for debugging purpose or once in a while.
     */
    void dump(ForwardRange<uint64_t> & range, const uint64_t imageHash) const;

    /**
     * code for this function is in ChainOStream.cpp
     * /!\ this is synchronized and will degrade performance seriously
     * use only for debugging purpose.
     */
    std::ostream& operator<<(std::ostream& stream) const;
};

#endif /* CHAIN_H_ */
