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

SmartCache::SmartCache(uint64_t limit, const ImageDecoderFactory& factory) :
    m_iSizeLimit(limit), m_CurrentMemory(0), m_Terminate(false) {

    if (limit != 0) {
        cout << "[ImageCache] Running image cache limited to " << (limit / 1024) / 1024 << "Mo\n";

        // Running 1:N load and 1:N decode thread separately isn't safe yet.
        // TODO: step1: Complete the OpenFX implementation to introduce the getInstance() method
        // TODO: step2: Apply the thread local storage method on all shared data used in IO plugins

        m_Chain.addWorker(boost::bind(&SmartCache::loadAndDecode, this, _1, boost::ref(factory)));
        //m_Chain.addWorker(boost::bind(&SmartCache::load, this, _1, boost::ref(factory)));
        //m_Chain.addWorker(boost::bind(&SmartCache::decode, this, _1, boost::ref(factory)));
    } else {
        m_Terminate = true;
    }
}

SmartCache::~SmartCache() {
    {
        boost::mutex::scoped_lock lock(m_CacheStateMutex);
        m_CurrentMemory = 0;
        m_Terminate = true;
    }
    m_LoadingCondition.notify_all();
}

void SmartCache::seek(ForwardRange<uint64_t> &range, const Chain::HashToFilenameFunction &function) {
    m_Chain.postNewJob(range, function, boost::bind(&SmartCache::computeTotalWeight, this, _1, _2));
    m_LoadingCondition.notify_one();
}

bool SmartCache::get(const uint64_t& hash, ImageHolder &imageHolder) const {
    Slot slot;
    if (!m_Chain.getResult(hash, slot)) {
        return false;
    }
    if (slot.m_pSlotData == NULL) {
        return false;
    }
    const TSlotDataPtr pData = boost::dynamic_pointer_cast<ASlotData>(slot.m_pSlotData);
    imageHolder = pData->m_Holder;
    return true;
}

void SmartCache::dump(ForwardRange<uint64_t>& range, const uint64_t &current) const {
    m_Chain.dump(range, current);
}

bool SmartCache::computeTotalWeight(const TChain& _tchain, uint64_t& totalSize) {
    totalSize = 0;
    BOOST_FOREACH(InternalSlot s, _tchain)
                {
                    const Slot shared = s.m_Shared;
                    switch (s.m_State) {
                        case NEW:
                        case LOADING:
                            break;
                        case LOADED:
                        case DECODING: {
                            const TSlotDataPtr pData = boost::dynamic_pointer_cast<ASlotData>(shared.m_pSlotData);
                            const ImageDescription &description = pData->m_TempImageDescription;
                            totalSize += description.imageDataSize;
                        }
                            break;
                        case READY: {
                            const TSlotDataPtr pData = boost::dynamic_pointer_cast<ASlotData>(shared.m_pSlotData);
                            ImageHolder &holder = pData->m_Holder;
                            totalSize += holder.getImageDataSize();
                            break;
                        }
                    }
                }

    {
        // updating current cache size
        boost::mutex::scoped_lock lock(m_CacheStateMutex);
        m_CurrentMemory = totalSize;
        //        cerr << "[" << (m_CurrentMemory / 1024.0) / 1024.0 << " Mo]" << endl;
    }

    return (totalSize < m_iSizeLimit);
}

void SmartCache::load(Chain& _c, const ImageDecoderFactory& factory) {
    try {
        while (true) {

            uint64_t hash = 0;
            try {
                TSlotDataPtr pData(new ASlotData());
                hash = getNextFilename(_c, pData);
                getImageHandler(factory, pData);
                if (pData->m_bDelegateReadToHost)
                    loadFileFromDisk(pData);
                readHeader(factory, pData);
                ImageDescription &imgDesc = pData->m_TempImageDescription;
                const size_t incomingDataSize = imgDesc.imageDataSize;
                {
                    boost::lock_guard<boost::mutex> lock(m_CacheStateMutex);
                    m_CurrentMemory += incomingDataSize;
                }
                // making room for the incoming element
                boost::unique_lock<boost::mutex> lock(m_LoadingMutex);
                while ((m_CurrentMemory + incomingDataSize >= m_iSizeLimit)) {
                    m_LoadingCondition.wait(lock);
                }
                {
                    boost::lock_guard<boost::mutex> lock(m_CacheStateMutex);
                    if (m_Terminate)
                        return;
                }
                _c.setLoadedSlot(Slot(hash, pData));
            } catch (load_error &e) {
                cerr << e.what() << endl;
                if (hash == 0)
                    return;
                _c.setDecodedSlot(Slot(hash));
            }

            //            // --- TEST
            //            const Slot::Shared shared = _c.getLoadHash();
            //            TSlotDataPtr pData(new ASlotData());
            //            // simulate read
            //            MemoryBlockPtr &pFile = pData->m_pFileMemoryBlock;
            //            MemoryBlockPtr pMemoryBlock(new MemoryBlock(&alignedMallocAlloc, 54 * 1024 * 1024));
            //            pFile = pMemoryBlock;
            //            // file infos
            //            ImageDescription &imgDesc = pData->m_TempImageDescription;
            //            imgDesc.pFileData = pFile->getPtr<char> ();
            //            imgDesc.fileDataSize = pFile->size();
            //            const size_t incomingDataSize = imgDesc.fileDataSize;
            //            {
            //                boost::lock_guard<boost::mutex> lock(m_CacheStateMutex);
            //                m_CurrentMemory += incomingDataSize;
            //            }
            //            boost::unique_lock<boost::mutex> lock(m_LoadingMutex);
            //            while ((m_CurrentMemory >= m_iSizeLimit)) {
            //                m_LoadingCondition.wait(lock);
            //            }
            //            _c.setLoaded(Slot::Shared(shared.m_ImageHash, pData));
        }
    } catch (chain_terminated &e) {
    }
}

void SmartCache::decode(Chain& _c, const ImageDecoderFactory& factory) {
    try {
        while (true) {

            uint64_t hash = 0;
            try {
                const Slot slot = _c.getDecodeSlot();
                hash = slot.m_ImageHash;
                TSlotDataPtr pData = boost::dynamic_pointer_cast<ASlotData>(slot.m_pSlotData);
                // readHeader(factory, pData);
                if (!isAlreadyUncompressed(pData)) {
                    readImage(factory, pData);
                }
                ImageDescription &description = pData->m_TempImageDescription;
                size_t incomingDataSize = description.imageDataSize;
                {
                    boost::lock_guard<boost::mutex> lock(m_CacheStateMutex);
                    m_CurrentMemory += incomingDataSize;
                }
                _c.setDecodedSlot(Slot(hash, pData));
            } catch (load_error &e) {
                cerr << e.what() << endl;
                if (hash == 0)
                    return;
                _c.setDecodedSlot(Slot(hash));
            }

            //            // --- TEST
            //            const Slot::Shared slot = _c.getDecodeHash();
            //            // sleep(millisec(40));
            //            const TSlotDataPtr pData = boost::dynamic_pointer_cast<ASlotData>(slot.m_pSlotData);
            //            ImageHolder &holder = pData->m_Holder;
            //            ImageDescription &description = pData->m_TempImageDescription;
            //            description.imageDataSize = description.fileDataSize;
            //            size_t incomingDataSize = description.imageDataSize;
            //            {
            //                boost::lock_guard<boost::mutex> lock(m_CacheStateMutex);
            //                m_CurrentMemory += incomingDataSize;
            //            }
            //            MemoryBlockPtr pImageMemoryBlock(new MemoryBlock(&alignedMallocAlloc, incomingDataSize));
            //            holder.setImageData(description, pImageMemoryBlock);
            //            _c.setDecoded(Slot::Shared(slot.m_ImageHash, pData));
        }
    } catch (chain_terminated &e) {
    }
}

void SmartCache::loadAndDecode(Chain& _c, const ImageDecoderFactory& factory) {
    try {
        while (true) {
            uint64_t hash = 0;
            try {
                // load
                {
                    TSlotDataPtr pData(new ASlotData());
                    hash = getNextFilename(_c, pData);
                    getImageHandler(factory, pData);
                    if (pData->m_bDelegateReadToHost)
                        loadFileFromDisk(pData);
                    readHeader(factory, pData);
                    ImageDescription &imgDesc = pData->m_TempImageDescription;
                    const size_t incomingDataSize = imgDesc.imageDataSize;
                    {
                        boost::lock_guard<boost::mutex> lock(m_CacheStateMutex);
                        m_CurrentMemory += incomingDataSize;
                    }
                    if (incomingDataSize >= (m_iSizeLimit / 2)) {
                        m_Terminate = true;
                        throw load_error("Insufficient cache size. Caching disabled.");
                    }
                    // making room for the incoming element
                    boost::unique_lock<boost::mutex> lock(m_LoadingMutex);
                    while ((m_CurrentMemory + incomingDataSize >= m_iSizeLimit)) {
                        m_LoadingCondition.wait(lock);
                    }
                    {
                        boost::lock_guard<boost::mutex> lock(m_CacheStateMutex);
                        if (m_Terminate)
                            return;
                    }
                    _c.setLoadedSlot(Slot(hash, pData));
                }
                // decode
                {
                    const Slot slot = _c.getDecodeSlot();
                    assert(slot.m_ImageHash == hash);
                    TSlotDataPtr pData = boost::dynamic_pointer_cast<ASlotData>(slot.m_pSlotData);
                    // readHeader(factory, pData);
                    if (!isAlreadyUncompressed(pData)) {
                        readImage(factory, pData);
                    }
                    ImageDescription &description = pData->m_TempImageDescription;
                    size_t incomingDataSize = description.imageDataSize;
                    {
                        boost::lock_guard<boost::mutex> lock(m_CacheStateMutex);
                        m_CurrentMemory += incomingDataSize;
                    }
                    _c.setDecodedSlot(Slot(hash, pData));
                }
            } catch (load_error &e) {
                cerr << e.what() << endl;
                if (hash == 0)
                    return;
                _c.setDecodedSlot(Slot(hash));
                if (m_Terminate)
                    return;
            }
        }
    } catch (chain_terminated &e) {
    }
}
