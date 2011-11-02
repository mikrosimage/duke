#include "ImageToolbox.h"
#include <dukeengine/file/StreamedFileIO.h>
#include <dukeengine/file/DmaFileIO.h>
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

const char * reader = "READ   : ";
const char * decoder = "DECODE : ";

uint64_t getNextFilename(Chain &chain, TSlotDataPtr& pData) {
    Slot slot = chain.getLoadSlot();
    assert(slot.m_ImageHash!=0);
    string filename;
    chain.getFilenameForHash(slot.m_ImageHash, filename);
    pData->m_Filename = filename;
    pData->m_FilenameExtension = boost::filesystem::path(filename).extension().string();
    //    cerr << reader << "hash " << shared.m_ImageHash << " filename " << pData->m_Filename << endl;
    return slot.m_ImageHash;
}

void getImageHandler(const ImageDecoderFactory& factory, TSlotDataPtr& pData) {
    pData->m_FormatHandler = factory.getImageDecoder(pData->m_FilenameExtension.c_str(), pData->m_bDelegateReadToHost, pData->m_bFormatUncompressed);
    if (pData->m_FormatHandler == NULL)
        throw load_error("no decoder for extension \"" + pData->m_FilenameExtension + "\"");
}

void loadFileFromDisk(TSlotDataPtr& pData) {
    // file reader
	::mikrosimage::alloc::Allocator *pAllocator = &_alignedMallocAlloc;
#ifdef WIN32
        DmaFileIO fileIO(pAllocator);
#else
        MappedFileIO fileIO(pAllocator);
#endif
    MemoryBlockPtr &pFile = pData->m_pFileMemoryBlock;
    pFile = fileIO.read(pData->m_Filename.c_str());
    if (pFile == NULL)
        throw load_error("unable to read " + pData->m_Filename);
    ImageDescription &imgDesc = pData->m_TempImageDescription;
    imgDesc.pFileData = pFile->getPtr<char> ();
    imgDesc.fileDataSize = pFile->size();
}

void readHeader(const ImageDecoderFactory& factory, TSlotDataPtr& pData) {
    if (!factory.readImageHeader(pData->m_Filename.c_str(), pData->m_FormatHandler, pData->m_TempImageDescription))
        throw load_error("unable to open " + pData->m_Filename);
}

void readImage(const ImageDecoderFactory& imageFactory, TSlotDataPtr& pData) {
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
bool isAlreadyUncompressed(TSlotDataPtr &pData) {
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
    TSlotDataPtr pData(new ASlotData());
    uint64_t hash = 0;
    try {
        hash = getNextFilename(chain, pData);
        getImageHandler(factory, pData);
        if (pData->m_bDelegateReadToHost)
            loadFileFromDisk(pData);
        //        chain.setLoaded(Slot::Shared(hash, pData));
    } catch (load_error &e) {
        cerr << e.what() << endl;
        if (hash == 0)
            return;
        const Slot shared(hash);
        chain.setDecodedSlot(Slot(hash));
    }
}

void decodeOne(Chain &chain, const ImageDecoderFactory& factory) {
    Slot slot = chain.getDecodeSlot();
    const uint64_t hash = slot.m_ImageHash;
    TSlotDataPtr pData = boost::dynamic_pointer_cast<ASlotData>(slot.m_pSlotData);
    try {
        readHeader(factory, pData);
        if (!isAlreadyUncompressed(pData))
            readImage(factory, pData);
        //        chain.setDecoded(Slot::Shared(hash, pData));
    } catch (load_error &e) {
        cerr << e.what() << endl;
        if (hash == 0)
            return;
        chain.setDecodedSlot(Slot(hash));
    }
}

//#if defined (WIN32)
//void setCpuAffinity(unsigned cpu) {
//    //SetThreadAffinityMask(GetCurrentThread(), 1 << cpu);
//}
//#else
//void setCpuAffinity(unsigned cpu) {
//}
//#endif
//
//void loadWorker(Chain &chain, const ImageDecoderFactory& factory, const unsigned cpu) {
//    setCpuAffinity(cpu);
//    try {
//        while (true) {
//            loadOne(chain, factory);
//        }
//    } catch (chain_terminated &e) {
//    }
//}
//
//void decodeWorker(Chain &chain, const ImageDecoderFactory& factory, const unsigned cpu) {
//    setCpuAffinity(cpu);
//    try {
//        while (true)
//            decodeOne(chain, factory);
//    } catch (chain_terminated &e) {
//    }
//}
