/*
 * ImageToolbox.cpp
 *
 *  Created on: 21 nov. 2011
 *      Author: Guillaume Chatelet
 */

#include "ImageToolbox.h"

#include <sequence/DisplayUtils.h>

#include <dukeengine/file/StreamedFileIO.h>
#include <dukeengine/file/MappedFileIO.h>
#include <dukeengine/file/DmaFileIO.h>
#include <dukeengine/file/MappedFileIO.h>

#include <dukeengine/host/io/ImageDecoderFactory.h>

using namespace std;

namespace image {
WorkUnitId::WorkUnitId(const duke::protocol::MediaFrame &mf) :
                duke::protocol::MediaFrame(mf) {
    boost::filesystem::path path;
    switch (mf.type) {
        case duke::protocol::Media_Type_SINGLE_IMAGE:
            path = mf.item.path;
            break;
        case duke::protocol::Media_Type_IMAGE_SEQUENCE:
            path = mf.item.path / sequence::instanciatePattern(mf.item.sequence.pattern, mf.source);
            break;
        default:
            filename = "can't decode movies for the moment";
            break;
    }
    filename = path.make_preferred().string();
}

bool WorkUnitId::operator==(const WorkUnitId &other) const {
    if(filename.size()!=other.filename.size())
        return false;
    return filename == other.filename;
}

bool WorkUnitId::operator!=(const WorkUnitId &other) const {
    return !operator ==(other);
}

bool WorkUnitId::operator<(const WorkUnitId &other) const {
    return filename < other.filename;
}

std::ostream& WorkUnitId::operator<<(std::ostream &stream) const {
    stream << '[' << index.frame << ':' << index.track << ']' << " -" << source << "- " << item;
    return stream;
}

using namespace ::mikrosimage::alloc;
AlignedMallocAllocator _alignedMallocAlloc;

static inline const char* extension(const string &filename) {
    const size_t found = filename.find_last_of(".");
    if (found == string::npos)
        return NULL;
    return filename.c_str() + found + 1;
}

/**
 * return true if file loaded
 */
static inline bool doLoadFile(WorkUnitData &unit) {
    // file reader
    ::mikrosimage::alloc::Allocator *pAllocator = &_alignedMallocAlloc;
#ifdef WIN32
    DmaFileIO fileIO(pAllocator);
//    StreamedFileIO fileIO(pAllocator);
#else
    MappedFileIO fileIO(pAllocator);
#endif
    MemoryBlockPtr &pFile = unit.pFileContent;
    pFile = fileIO.read(unit.id.filename.c_str());
    if (pFile == NULL)
        return false;
    ImageDescription &imgDesc = unit.imageDescription;
    imgDesc.pFileData = pFile->getPtr<char>();
    imgDesc.fileDataSize = pFile->size();
    assert(imgDesc.pImageData == NULL);
    assert(imgDesc.imageDataSize == 0);
    return true;
}

bool load(const ImageDecoderFactory& factory, WorkUnitData &unit, uint64_t& size) {
    size = 0;
    const string& filename = unit.id.filename;
    string &error = unit.imageHolder.error;
    if (filename.empty()) {
        error = "filename is empty";
        return true; // image is unreachable
    }
    const char* pExtension = extension(filename);
    if (pExtension == NULL) {
        error = "filename has no extension";
        return true;
    }
    bool delegateReadToHost, isUncompressed;
    FormatHandle &pHandle = unit.pFormatHandler;
    pHandle = factory.getImageDecoder(pExtension, delegateReadToHost, isUncompressed);
    if (pHandle == NULL) {
        error = "no decoder found";
        return true;
    }
    if (!delegateReadToHost)
        return false; // the decoder will have to load it
    if (!doLoadFile(unit)) {
        error = "unable to read file";
        return true;
    }
    return false; // still need to decode it
}

// if pImageData is set, it means the uncompressed data was in the file in which case we must
// save the allocated memory along with the image ( ie : in the ImageHolder )
static inline bool isAlreadyUncompressed(WorkUnitData &unit, uint64_t& size) {
    const ImageDescription &description = unit.imageDescription;
    if (description.pImageData == NULL)
        return false;
    const MemoryBlockPtr &pFileMemoryBlock = unit.pFileContent;
    assert( pFileMemoryBlock->hold(description.pImageData));
    assert( pFileMemoryBlock->hold(description.pImageData + description.imageDataSize));
    unit.imageHolder.setImageData(description, pFileMemoryBlock);
    size = pFileMemoryBlock->size();
    return true;
}

static inline void readImage(const ImageDecoderFactory& imageFactory, WorkUnitData& unit, uint64_t& size) {
    const ImageDescription &description = unit.imageDescription;
    ImageHolder &holder = unit.imageHolder;
    assert( description.imageDataSize > 0);
    MemoryBlockPtr pImageMemoryBlock(new MemoryBlock(&_alignedMallocAlloc, description.imageDataSize));
    holder.setImageData(description, pImageMemoryBlock);
    assert( unit.pFormatHandler);
    if (imageFactory.decodeImage(unit.pFormatHandler, holder.getImageDescription())) {
        size = holder.getImageDataSize();
    } else {
        unit.imageHolder.error = "unable to decode '" + unit.id.filename + "'";
    }
    // we can release the file's memory
    unit.pFileContent.reset();
}

void decode(const ImageDecoderFactory &factory, WorkUnitData &unit, uint64_t& size) {
    // trying to decode header
    if (!factory.readImageHeader(unit.pFormatHandler, unit.id.filename.c_str(), unit.imageDescription)) {
        unit.imageHolder.error = "unable to decode header for '" + unit.id.filename + "'";
        return;
    }
    if (!isAlreadyUncompressed(unit, size))
        readImage(factory, unit, size);
}

} // namespace image

std::ostream& operator<<(std::ostream &stream, const image::WorkUnitId&id ){
    return id.operator <<(stream);
}
