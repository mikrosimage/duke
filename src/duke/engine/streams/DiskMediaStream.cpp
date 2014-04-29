#include "DiskMediaStream.hpp"

#include <duke/base/Check.hpp>
#include <duke/base/StringUtils.hpp>
#include <duke/attributes/AttributeKeys.hpp>
#include <duke/memory/Allocator.hpp>
#include <duke/filesystem/FsUtils.hpp>
#include <duke/engine/ImageLoadUtils.hpp>
#include <duke/engine/streams/MediaFrameReference.hpp>
#include <duke/imageio/DukeIO.hpp>

#include <algorithm>
#include <memory>

namespace duke {

namespace {

std::vector<IIODescriptor*> findIODescriptors(const sequence::Item& item) {
    const auto& filename = item.filename;
    const char* pExtension = fileExtension(filename.c_str());
    const auto& descriptors = IODescriptors::instance().findDescriptor(pExtension);
    return {begin(descriptors), end(descriptors)};
}

bool isFileSequenceReader(const IIODescriptor* pDescriptor){
    return pDescriptor->supports(IIODescriptor::Capability::READER_FILE_SEQUENCE);
}

std::unique_ptr<IImageReader> getFirstValidReader(const attribute::Attributes& options, const std::vector<IIODescriptor*>& descriptors, const char* filename) {
    std::unique_ptr<IImageReader> pCurrent;
    for (const auto* pDescriptor : descriptors) {
        pCurrent.reset(pDescriptor->getReaderFromFile(options, filename));
        if(!pCurrent) {
        	printf("Discarding '%s' reader, read from memory is not yet implemented\n", pDescriptor->getName());
        	continue;
        }
        if (pCurrent->hasError() == false) return pCurrent;
    }
    return pCurrent;
}

BigAlignedBlock gBigAlignedMallocator;

void CopyDataFromVolatilePointer(RawPackedFrame& packedFrame, const void* pVolatileData) {
    if (!packedFrame.pData) {
        const size_t dataSize = packedFrame.description.dataSize;
        packedFrame.pData = make_shared_memory<char>(dataSize, gBigAlignedMallocator);
        memcpy(packedFrame.pData.get(), pVolatileData, dataSize);
    }
}

class FileSequenceImpl : public IMediaStreamDelegate {
public:
    FileSequenceImpl(const attribute::Attributes& options, const sequence::Item& item, attribute::Attributes& state) :
                    m_Item(item),
                    m_ItemType(m_Item.getType()),
                    m_Descriptors(findIODescriptors(item)),
                    m_Options(options) {
        CHECK(m_ItemType == sequence::Item::PACKED);
        CHECK(std::all_of(begin(m_Descriptors), end(m_Descriptors), &isFileSequenceReader));
        const auto& filename = item.filename;
        const auto begin = filename.begin();
        auto firstSharpIndex = filename.find('#');
        m_Prefix = std::string(begin, begin + firstSharpIndex);
        auto lastSharpIndex = filename.rfind('#');
        m_Suffix = std::string(begin + lastSharpIndex + 1, filename.end());
        frameCount = item.end - item.start + 1;
    }

    // Several threads will access this function at the same time.
    virtual InputFrameOperationResult process(const size_t frame) const override {
        InputFrameOperationResult result;
        attribute::set<attribute::File>(result.attributes(), generateFilePath(frame).c_str());
        return duke::load(m_Options, &CopyDataFromVolatilePointer, std::move(result));
    }
private:
    std::string generateFilePath(size_t atFrame) const {
        return writeFilename(atFrame + m_Item.start);
    }

    std::string writeFilename(size_t frame) const {
        const size_t paddingSize = m_Item.padding > 0 ? m_Item.padding : digits(frame);
        const size_t bufferSize = m_Prefix.size() + paddingSize + m_Suffix.size();
        std::string path;
        path.reserve(bufferSize);
        path += m_Prefix;
        appendPaddedFrameNumber(frame, paddingSize, path);
        path += m_Suffix;
        return path;
    }

    sequence::Item m_Item;
    sequence::Item::Type m_ItemType;
    std::string m_Prefix;
    std::string m_Suffix;
    std::vector<IIODescriptor*> m_Descriptors;
    attribute::Attributes m_Options;
};

class SingleFileImpl : public IMediaStreamDelegate {
public:
    SingleFileImpl(const attribute::Attributes& options, const sequence::Item& item, attribute::Attributes& state) :
                    m_Descriptors(findIODescriptors(item)),
                    m_Filename(item.filename),
                    m_pImageReader(getFirstValidReader(options, m_Descriptors, m_Filename.c_str())) {
        if (!m_pImageReader) {
            std::string error = "No reader for '";
            error += m_Filename;
            error += "'";
            attribute::set<attribute::Error>(state, error.c_str());
            return;
        }
        if (m_pImageReader->hasError()) {
          attribute::set<attribute::Error>(state, m_pImageReader->getError().c_str());
            m_pImageReader.reset();
            return;
        }
        frameCount = attribute::getWithDefault<attribute::MediaFrameCount>(m_pImageReader->getAttributes(), 1);
    }

    virtual InputFrameOperationResult process(const size_t frame) const override {
        InputFrameOperationResult result;
        attribute::set<attribute::File>(result.attributes(), m_Filename.c_str());
        attribute::set<attribute::MediaFrame>(result.attributes(), frame);
        CHECK(m_pImageReader);
        std::lock_guard<std::mutex> guard(m_Mutex);
        return duke::loadImage(m_pImageReader.get(), &CopyDataFromVolatilePointer, std::move(result));
    }
private:
    const std::vector<IIODescriptor*> m_Descriptors;
    const std::string m_Filename;
    std::unique_ptr<IImageReader> m_pImageReader;
    mutable std::mutex m_Mutex;
};


}  // namespace


DiskMediaStream::DiskMediaStream(const attribute::Attributes& options, const sequence::Item& item) : m_IsForward(false) {
    switch (item.getType()) {
        case sequence::Item::SINGLE:
            m_pDelegate.reset(new SingleFileImpl(options, item, m_StreamAttributes));
            m_IsForward = true;
            break;
        case sequence::Item::PACKED:
            m_pDelegate.reset(new FileSequenceImpl(options, item, m_StreamAttributes));
            m_IsForward = false;
            break;
        default:
            CHECK(!"Invalid state");
            break;
    }
    CHECK(m_pDelegate);
    if (attribute::contains<attribute::Error>(m_StreamAttributes))
        return;
    attribute::set<attribute::MediaFrameCount>(m_StreamAttributes, m_pDelegate->frameCount);
}

} /* namespace duke */
