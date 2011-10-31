#include "SmartCache.h"
#include <dukeengine/image/ImageToolbox.h>
#include <dukeengine/memory/alloc/MemoryBlock.h> //todelete
#include <dukeengine/memory/alloc/Allocators.h> //todelete
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace ::boost::this_thread;
using namespace ::boost::posix_time;

namespace { //empty

::mikrosimage::alloc::AlignedMallocAllocator alignedMallocAlloc;

} //empty

SmartCache::SmartCache(size_t limit, const ImageDecoderFactory& factory) :
    m_iSizeLimit(limit) {
    if (isActive()) {
        cout << "[ImageCache] Running image cache limited to " << (limit / 1024) / 1024 << "Mo\n";

        // Running 1:N load and 1:N decode thread separately isn't safe yet.
        // TODO: step1: Complete the OpenFX implementation to introduce the getInstance() method
        // TODO: step2: Apply the thread local storage method on all shared data used in IO plugins

        //        m_Chain.addWorker(boost::bind(&SmartCache::loadAndDecode, this, _1, boost::ref(factory)));
        addWorker(boost::bind(&loadWorker, _1, boost::ref(factory), 0));
        addWorker(boost::bind(&decodeWorker, _1, boost::ref(factory), 0));
    }
}

void SmartCache::seek(ForwardRange<uint64_t> &range, const Chain::HashToFilenameFunction &function) {
    postNewJob(range, function/*, boost::bind(&SmartCache::computeTotalWeight, this, _1, _2)*/);
}

bool SmartCache::get(const uint64_t& hash, ImageHolder &imageHolder) const {
    Slot slot;
    if (!getResult(hash, slot) || slot.m_pSlotData == NULL)
        return false;

    const TSlotDataPtr pData = boost::dynamic_pointer_cast<DukeSlot>(slot.m_pSlotData);
    imageHolder = pData->m_Holder;
    return true;
}

static inline size_t weight(const InternalSlot& slot) {
    const Slot shared = slot.m_Shared;
    switch (slot.m_State) {
        case NEW:
        case LOADING:
        case UNDEFINED:
            return 0;
        default:
            break;
    }
    const TSlotDataPtr pData = boost::dynamic_pointer_cast<DukeSlot>(shared.m_pSlotData);
    if (!pData)
        return 0;
    return pData->m_TempImageDescription.fileDataSize + pData->m_TempImageDescription.imageDataSize;
}

size_t SmartCache::getEvictionIterator(const TChain& chain) const {
    size_t currentIndex = 0;
    size_t memory = 0;
    for (TChain::const_iterator itr = chain.begin(); itr != chain.end(); ++itr, ++currentIndex) {
        const size_t currentWeight = weight(*itr);
        memory += currentWeight;
        if (memory > m_iSizeLimit)
            return currentIndex;
    }
    return chain.size();
}
