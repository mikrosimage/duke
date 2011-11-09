#include "SmartCache.h"

#include <dukeengine/range/PlaylistRange.h>
#include <dukeengine/sequence/PlaylistHelper.h>
#include <dukeengine/image/ImageToolbox.h>
#include <dukeengine/file/DmaFileIO.h>
#include <dukeengine/file/MappedFileIO.h>
#include <dukeengine/memory/alloc/Allocators.h>
#include <dukeengine/memory/alloc/BigAlignedBlockAllocator.h>

#include <boost/foreach.hpp>

#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;
using namespace ::boost::this_thread;
using namespace ::boost::posix_time;

SmartCache::SmartCache(uint64_t limit, const ImageDecoderFactory& factory) :
    m_CacheSizeLimit(limit), m_ImageDecoderFactory(factory), m_pCurrentHelper(NULL) {
    if (isActive()) {
        cout << "[ImageCache] Running image cache limited to " << (limit / 1024) / 1024 << "Mo\n";

        // Running 1:N load and 1:N decode thread separately isn't safe yet.
        // TODO: step1: Complete the OpenFX implementation to introduce the getInstance() method
        // TODO: step2: Apply the thread local storage method on all shared data used in IO plugins

        //        m_Chain.addWorker(boost::bind(&SmartCache::loadAndDecode, this, _1, boost::ref(factory)));
        addWorker(boost::bind(&loadWorker, _1, boost::ref(factory), 0));
        addWorker(boost::bind(&decodeWorker, _1, boost::ref(factory), 1));
        addWorker(boost::bind(&decodeWorker, _1, boost::ref(factory), 2));
        addWorker(boost::bind(&decodeWorker, _1, boost::ref(factory), 3));
    }
}

void SmartCache::seek(std::size_t frame, uint32_t speed, const PlaylistHelper *pHelper) {
    assert(pHelper!=NULL);
    m_pCurrentHelper = pHelper;
    if(!isActive()) // TODO if same frame don't post a new job
        return;
//    using range::PlaylistFrameRange;
//    const ptrdiff_t balancingBound = speed == 0 ? 0 : (speed > 0 ? -5 : 5);
//    PlaylistFrameRange range(m_pCurrentHelper->getFrameCount(), frame, balancingBound, speed < 0);
//    Chain::postNewJob<PlaylistFrameRange>(range, boost::bind(&PlaylistHelper::getPathStringAtHash, getSharedPlaylistHelper(), _1));
}

//static bool get(const ImageReader& imageReader, const PlaylistHelper &helper, const size_t itrIndex, ImageHolder& value) {
//    const uint64_t hash = helper.getHashAtIterator(itrIndex);
//    if (hash == 0)
//        return false;
//
//    const boost::filesystem::path filename = helper.getPathAtHash(hash);
//    if (filename.empty())
//        return true;
//
//    return imageReader.load(filename, value);
//}

using namespace ::mikrosimage::alloc;

::mikrosimage::alloc::AlignedMallocAllocator alignedMallocAlloc;
::mikrosimage::alloc::BigAlignedBlockAllocator bigAlignedBlockAlloc;

void read(const ImageDecoderFactory& imageFactory, const boost::filesystem::path &path, ImageHolder &holder, FormatHandle formatHandler, const ImageDescription& description) {
    assert( description.imageDataSize > 0 );
    MemoryBlockPtr pImageMemoryBlock(new MemoryBlock(&bigAlignedBlockAlloc, description.imageDataSize));
    holder.setImageData(description, pImageMemoryBlock);
    if (!imageFactory.decodeImage(formatHandler, holder.getImageDescription()))
        std::cerr << "unable to decode " << path << std::endl;
}

bool load(const ImageDecoderFactory& m_ImageFactory, const boost::filesystem::path &path, ImageHolder &holder) {
    bool isFormatUncompressed;
    bool delegateReadToHost;
    const FormatHandle formatHandler = m_ImageFactory.getImageDecoder(path.extension().string().c_str(), delegateReadToHost, isFormatUncompressed);

    if (formatHandler == NULL) {
        std::cerr << "no decoder for extension \"" << path.extension() << "\"" << std::endl;
        return false;
    }

    ImageDescription description;
    const std::string filename = path.string();
    const char* c_filename = filename.c_str();

    if (delegateReadToHost) {
        // file reader
        ::mikrosimage::alloc::Allocator *pAllocator = &alignedMallocAlloc;
        if (path.extension() == ".dpx")
            pAllocator = &bigAlignedBlockAlloc;
#ifdef WIN32
        DmaFileIO fileIO(pAllocator);
#else
        MappedFileIO fileIO(pAllocator);
#endif

        // reading file in memory
        const MemoryBlockPtr pFileMemoryBlock = fileIO.read(c_filename);
        //        m_TimeReadFile = file.splitTime();
        if (pFileMemoryBlock == NULL) {
            std::cerr << "unable to read " << path << std::endl;
            return false;
        }

        // setting read file in description
        description.pFileData = pFileMemoryBlock->getPtr<char> ();
        description.fileDataSize = pFileMemoryBlock->size();

        // reading header
        if (!m_ImageFactory.readImageHeader(formatHandler, c_filename, description)) {
            std::cerr << "unable to open " << path << std::endl;
            return false;
        }
        assert( description.imageDataSize > 0 );
        // if pImageData is set, it means the uncompressed data was in the file in which case we must
        // save the allocated memory along with the image ( ie : in the ImageHolder )
        if (description.pImageData != NULL) {
            assert( isFormatUncompressed );
            assert( pFileMemoryBlock->hold(description.pImageData) );
            assert( pFileMemoryBlock->hold(description.pImageData + description.imageDataSize) );
            holder.setImageData(description, pFileMemoryBlock);
            return true;
        }
        //        StopWatch decode(true);
        read(m_ImageFactory, path, holder, formatHandler, description);
    } else {
        if (!m_ImageFactory.readImageHeader(formatHandler, c_filename, description)) {
            std::cerr << "unable to open " << path << std::endl;
            return false;
        }
        read(m_ImageFactory, path, holder, formatHandler, description);
    }
    return true;
}

bool SmartCache::get(size_t playlistItr, ImageHolder &imageHolder) const {
    assert(m_pCurrentHelper!=NULL);
    const uint64_t hash = m_pCurrentHelper->getHashAtIterator(playlistItr);
    if (hash == 0)
        return false; // no frame here, returning false

    if (isActive()) {
        Slot slot;
        if (!getResult(hash, slot) || slot.m_pSlotData == NULL)
            return false;

        const TSlotDataPtr pData = boost::dynamic_pointer_cast<DukeSlot>(slot.m_pSlotData);
        imageHolder = pData->m_Holder;
        return true;
    } else {
        const boost::filesystem::path filename = m_pCurrentHelper->getPathAtHash(hash);
        if (filename.empty())
            return true;

        return load(m_ImageDecoderFactory, filename, imageHolder);
    }
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

size_t SmartCache::getNewEndIndex(const TChain& chain) const {
    size_t currentIndex = 0;
    size_t memory = 0;
    for (TChain::const_iterator itr = chain.begin(); itr != chain.end(); ++itr, ++currentIndex) {
        const size_t currentWeight = weight(*itr);
        memory += currentWeight;
        if (memory > m_CacheSizeLimit)
            return currentIndex;
    }
    return chain.size();
}
