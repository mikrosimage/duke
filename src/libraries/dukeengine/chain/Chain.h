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

struct Slot {
private:
    typedef boost::shared_ptr<SlotData> TSlotDataPtr;
public:
    enum State {
        NEW = 0, LOADING = 1, LOADED = 2, DECODING = 3, READY = 4
    };
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
    struct Shared {
        uint64_t m_ImageHash;
        TSlotDataPtr m_pSlotData;
        Shared() :
            m_ImageHash(0) {
        }
        Shared(uint64_t hash) :
            m_ImageHash(hash) {
        }
        Shared(uint64_t hash, const TSlotDataPtr &ptr) :
            m_ImageHash(hash), m_pSlotData(ptr) {
        }
    } m_Shared;
    State m_State;

    Slot();
    Slot(uint64_t imageHash);
    Slot(uint64_t imageHash, State state);
    bool operator<(const Slot& other) const;

}__attribute__((aligned (32)));
// ensuring the struct is aligned to maximize cache hit

struct TChain : public std::vector<Slot> {
    const static size_t InitialChainReserve = 100;
    TChain();
    TChain(OnePassRange<uint64_t> &iterator);
    iterator quickFind(const Slot::State state, size_t &fromIndex);
    iterator find(const uint64_t);
    const_iterator find(const uint64_t) const;
    const_iterator safeFind(const uint64_t) const;
};

void transferWorkUnit(TChain &from, TChain &to, bool avoidEviction);

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
 * You can inherit from this struct to put data in the chain
 * data will be read/only but accessible from the workers
 */
struct ChainContext : boost::noncopyable {
    virtual ~ChainContext()=0;
};

class Chain : boost::noncopyable {
public:
    typedef boost::function<std::string(uint64_t)> HashToFilenameFunction;
    typedef boost::function<bool(const TChain &, uint64_t &)> ComputeTotalWeightFunction;
    typedef boost::function<void(Chain&)> WorkerThreadFunction;
    typedef std::vector<WorkerThreadFunction> WorkerThreadFunctions;
private:
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

    Slot::Shared getHash(const Slot::State, const Slot::State, boost::condition&);
    void setData(const Slot::Shared &, const Slot::State, boost::condition&);
    void stopWorkers();
    inline void cleanAccelerators() {
        m_LastIndexAccelerator[0] = 0;
        m_LastIndexAccelerator[1] = 0;
    }
public:
    Chain();
    ~Chain();
    /**
     * post a new job in the form of an index range
     * /!\ !!! range must not contains twice the same index !!!
     * This precondition is checked in debug mode
     */
    void postNewJob(ForwardRange<uint64_t> &range, const HashToFilenameFunction &function, const ComputeTotalWeightFunction &weighfunction);

    /**
     * append workers to this Chain to multithread load and decode
     */
    void addWorker(const WorkerThreadFunction &function);
    void addWorker(const WorkerThreadFunctions &functions);

    // returns the time needed to decode 'count' images with 'functions' instantiated as workers
    boost::posix_time::time_duration benchmark(const WorkerThreadFunctions &functions, const HashToFilenameFunction &function, const size_t count);

    /**
     * worker functions to fill the chain
     */
    inline Slot::Shared getLoadHash() {
        return getHash(Slot::NEW, Slot::LOADING, m_condNewJob);
    }
    inline Slot::Shared getDecodeHash() {
        return getHash(Slot::LOADED, Slot::DECODING, m_condSlotLoaded);
    }
    inline void setLoaded(Slot::Shared shared) {
        setData(shared, Slot::LOADED, m_condSlotLoaded);
    }
    inline void setDecoded(Slot::Shared shared) {
        setData(shared, Slot::READY, m_condSlotDecoded);
    }
    void getFilenameForHash(const uint64_t &hash, std::string &filename) const;

    /**
     * return true if slot is updated with the image
     * return false if no worker can fulfill the job
     * throw if asking for an image out the provided job's range
     */
    bool getResult(const uint64_t &imageHash, Slot::Shared &slot) const;

    void dump(ForwardRange<uint64_t> & range, const uint64_t &imageHash) const;

    /**
     * code for this function is in ChainOStream.cpp
     * /!\ this is synchronized and will degrade performance seriously
     * use only when debugging.
     */
    std::ostream& operator<<(std::ostream& stream) const;
};

#endif /* CHAIN_H_ */
