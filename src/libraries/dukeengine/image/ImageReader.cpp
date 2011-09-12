#include "ImageReader.h"
#include <dukeengine/file/DmaFileIO.h>
#include <dukeengine/file/MappedFileIO.h>
#include <dukeengine/file/StreamedFileIO.h>
#include <dukeengine/host/io/ImageDecoderFactory.h>
#include <iostream>

using namespace ::mikrosimage::alloc;

::mikrosimage::alloc::AlignedMallocAllocator alignedMallocAlloc;
::mikrosimage::alloc::BigAlignedBlockAllocator bigAlignedBlockAlloc;
//MallocAllocator mallocAlloc;
//NewAllocator newAlloc;
//VirtualAllocator virtualAlloc;

ImageReader::ImageReader(const ImageDecoderFactory& imageFactory) :
    m_ImageFactory(imageFactory) {
}

ImageReader::~ImageReader() {
}

void read(const ImageDecoderFactory& imageFactory, const boost::filesystem::path &path, ImageHolder &holder, FormatHandle formatHandler, const ImageDescription& description) {
    assert( description.imageDataSize > 0 );
    MemoryBlockPtr pImageMemoryBlock(new MemoryBlock(&bigAlignedBlockAlloc, description.imageDataSize));
    holder.setImageData(description, pImageMemoryBlock);
    if (!imageFactory.decodeImage(formatHandler, holder.getImageDescription()))
        std::cerr << "unable to decode " << path << std::endl;
}

bool ImageReader::load(const boost::filesystem::path &path, ImageHolder &holder) const {
//    m_TimeWhole = boost::posix_time::time_duration();
//    m_TimeGetDecoder = boost::posix_time::time_duration();
//    m_TimeReadFile = boost::posix_time::time_duration();
//    m_TimeReadHeader = boost::posix_time::time_duration();
//    m_TimeDecode = boost::posix_time::time_duration();
//    StopWatch whole(true);

    bool isFormatUncompressed;
    bool delegateReadToHost;
//    StopWatch getDecoder(true);
    const FormatHandle formatHandler = m_ImageFactory.getImageDecoder(path.extension().string().c_str(), delegateReadToHost, isFormatUncompressed);
//    m_TimeGetDecoder = getDecoder.splitTime();

    if (formatHandler == NULL) {
        std::cerr << "no decoder for extension \"" << path.extension() << "\"" << std::endl;
        return false;
    }

    ImageDescription description;
    const char* filename(path.string().c_str());

    if (delegateReadToHost) {
        // file reader
        ::mikrosimage::alloc::Allocator *pAllocator = &alignedMallocAlloc;
        if(path.extension()==".dpx")
            pAllocator = &bigAlignedBlockAlloc;
#ifdef false//WIN32
        DmaFileIO fileIO(pAllocator);
#else
        MappedFileIO fileIO(pAllocator);
#endif

//        StopWatch file(true);
        // reading file in memory
        const MemoryBlockPtr pFileMemoryBlock = fileIO.read(filename);
//        m_TimeReadFile = file.splitTime();
        if (pFileMemoryBlock == NULL) {
            std::cerr << "unable to read " << path << std::endl;
            return false;
        }

        // setting read file in description
        description.pFileData = pFileMemoryBlock->getPtr<char> ();
        description.fileDataSize = pFileMemoryBlock->size();

//        StopWatch header(true);
        // reading header
        if (!m_ImageFactory.readImageHeader(filename, formatHandler, description)) {
            std::cerr << "unable to open " << path << std::endl;
            return false;
        }
//        m_TimeReadHeader = header.splitTime();

        assert( description.imageDataSize > 0 );
        // if pImageData is set, it means the uncompressed data was in the file in which case we must
        // save the allocated memory along with the image ( ie : in the ImageHolder )
        if (description.pImageData != NULL) {
            assert( isFormatUncompressed );
            assert( pFileMemoryBlock->hold(description.pImageData) );
            assert( pFileMemoryBlock->hold(description.pImageData + description.imageDataSize) );
            holder.setImageData(description, pFileMemoryBlock);
//            m_TimeWhole = whole.splitTime();
            return true;
        }
//        StopWatch decode(true);
        read(m_ImageFactory, path, holder, formatHandler, description);
//        m_TimeDecode = decode.splitTime();
    } else {
//        StopWatch header(true);
        if (!m_ImageFactory.readImageHeader(filename, formatHandler, description)) {
            std::cerr << "unable to open " << path << std::endl;
            return false;
        }
//        m_TimeReadHeader = header.splitTime();
//        StopWatch decode(true);
        read(m_ImageFactory, path, holder, formatHandler, description);
//        m_TimeDecode = decode.splitTime();
    }
//    m_TimeWhole = whole.splitTime();
    return true;
}

void ImageReader::displayLastLoadStats() const {
    using namespace std;

    cout << "whole : ";
    cout.width(5);
    cout << m_TimeWhole.total_milliseconds();
    cout << "ms";
    cout << " getDecoder : ";
    cout.width(5);
    cout << m_TimeGetDecoder.total_milliseconds();
    cout << "ms";
    cout << " readFile : ";
    cout.width(5);
    cout << m_TimeReadFile.total_milliseconds();
    cout << "ms";
    cout << " readHeader : ";
    cout.width(5);
    cout << m_TimeReadHeader.total_milliseconds();
    cout << "ms";
    cout << " decode : ";
    cout.width(5);
    cout << m_TimeDecode.total_milliseconds();
    cout << "ms\n";
}
