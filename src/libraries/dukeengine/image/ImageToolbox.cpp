#include "ImageToolbox.h"
#include <dukeengine/file/StreamedFileIO.h>
#include <iostream>

using namespace ::mikrosimage::alloc;
AlignedMallocAllocator _alignedMallocAlloc;

using namespace std;

PlaylistHashToName::PlaylistHashToName(const SharedPlaylistHelperPtr &playlistHelper) :
    playlistHelper(playlistHelper) {
}

string PlaylistHashToName::getFilename(uint64_t hash) const {
    return playlistHelper->getPathAtHash(hash).string();
}

struct load_error : public runtime_error {
    load_error(string what) :
        runtime_error(what) {
    }
};

const char * reader = "READ   : ";
const char * decoder = "DECODE : ";

inline uint64_t getNextFilename(Chain &chain, TSlotDataPtr& pData) {
    Slot::Shared shared = chain.getLoadHash();
    assert(shared.m_ImageHash!=0);
    string filename;
    chain.getFilenameForHash(shared.m_ImageHash, filename);
    pData->m_Filename = filename;
    pData->m_FilenameExtension = boost::filesystem::path(filename).extension().string();
    //    cerr << reader << "hash " << shared.m_ImageHash << " filename " << pData->m_Filename << endl;
    return shared.m_ImageHash;
}

inline void getImageHandler(const ImageDecoderFactory& factory, TSlotDataPtr& pData) {
    pData->m_FormatHandler = factory.getImageDecoder(pData->m_FilenameExtension.c_str(), pData->m_bDelegateReadToHost, pData->m_bFormatUncompressed);
    if (pData->m_FormatHandler == NULL)
        throw load_error("no decoder for extension \"" + pData->m_FilenameExtension + "\"");
}

inline void loadFileFromDisk(TSlotDataPtr& pData) {
    // file reader
    StreamedFileIO fileIO(&_alignedMallocAlloc);
    MemoryBlockPtr &pFile = pData->m_pFileMemoryBlock;
    pFile = fileIO.read(pData->m_Filename.c_str());
    if (pFile == NULL)
        throw load_error("unable to read " + pData->m_Filename);
    ImageDescription &imgDesc = pData->m_TempImageDescription;
    imgDesc.pFileData = pFile->getPtr<char> ();
    imgDesc.fileDataSize = pFile->size();
}

inline void readHeader(const ImageDecoderFactory& factory, TSlotDataPtr& pData) {
    if (!factory.readImageHeader(pData->m_Filename.c_str(), pData->m_FormatHandler, pData->m_TempImageDescription))
        throw load_error("unable to open " + pData->m_Filename);
}

inline void readImage(const ImageDecoderFactory& imageFactory, TSlotDataPtr& pData) {
    const ImageDescription &description = pData->m_TempImageDescription;
    ImageHolder &holder = pData->m_Holder;
    assert( description.imageDataSize > 0 );
    MemoryBlockPtr pImageMemoryBlock(new MemoryBlock(&_alignedMallocAlloc, description.imageDataSize));
    holder.setImageData(description, pImageMemoryBlock);
    assert( pData->m_FormatHandler );
    if (!imageFactory.decodeImage(pData->m_FormatHandler, holder.getImageDescription()))
        throw load_error(string("unable to decode ") + pData->m_Filename);
}

// if pImageData is set, it means the uncompressed data was in the file in which case we must
// save the allocated memory along with the image ( ie : in the ImageHolder )
inline bool isAlreadyUncompressed(TSlotDataPtr &pData) {
    const ImageDescription &description = pData->m_TempImageDescription;
    if (description.pImageData == NULL)
        return false;
    const MemoryBlockPtr &pFileMemoryBlock = pData->m_pFileMemoryBlock;
    assert( pData->m_bFormatUncompressed );
    assert( pFileMemoryBlock->hold(description.pImageData) );
    assert( pFileMemoryBlock->hold(description.pImageData + description.imageDataSize) );
    pData->m_Holder.setImageData(description, pFileMemoryBlock);
    return true;
}

void loadOne(Chain &chain, const ImageDecoderFactory& factory) {
//    StopWatch time(true);
    TSlotDataPtr pData(new ASlotData());
    uint64_t hash = 0;
    try {
        hash = getNextFilename(chain, pData);
        getImageHandler(factory, pData);
        if (pData->m_bDelegateReadToHost)
            loadFileFromDisk(pData);
//        pData->loadTime = time.splitTime();
        //        cerr << reader << "read " << pData->loadTime << endl;
        chain.setLoaded(Slot::Shared(hash, pData));
    } catch (load_error &e) {
        cerr << e.what() << endl;
        if (hash == 0)
            return;
        const Slot::Shared shared(hash);
        chain.setDecoded(Slot::Shared(hash));
    }
}

void decodeOne(Chain &chain, const ImageDecoderFactory& factory) {
//    StopWatch time(true);
    Slot::Shared shared = chain.getDecodeHash();
    const uint64_t hash = shared.m_ImageHash;
    TSlotDataPtr pData = boost::dynamic_pointer_cast<ASlotData>(shared.m_pSlotData);
    try {
        readHeader(factory, pData);
        if (!isAlreadyUncompressed(pData))
            readImage(factory, pData);
//        pData->decodeTime = time.splitTime();
        //        cerr << decoder << "decoded " << pData->decodeTime << endl;
        chain.setDecoded(Slot::Shared(hash, pData));
    } catch (load_error &e) {
        cerr << e.what() << endl;
        if (hash == 0)
            return;
        chain.setDecoded(Slot::Shared(hash));
    }
}

#if defined (WIN32)
void setCpuAffinity(unsigned cpu) {
    //SetThreadAffinityMask(GetCurrentThread(), 1 << cpu);
}
#else
void setCpuAffinity(unsigned cpu) {
}
#endif

void loadWorker(Chain &chain, const ImageDecoderFactory& factory, const unsigned cpu) {
    setCpuAffinity(cpu);
    try {
        while (true) {
            loadOne(chain, factory);
        }
    } catch (chain_terminated &e) {
    }
}

void decodeWorker(Chain &chain, const ImageDecoderFactory& factory, const unsigned cpu) {
    setCpuAffinity(cpu);
    try {
        while (true)
            decodeOne(chain, factory);
    } catch (chain_terminated &e) {
    }
}
