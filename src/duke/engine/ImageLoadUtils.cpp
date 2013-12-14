#include "ImageLoadUtils.hpp"
#include <duke/imageio/DukeIO.hpp>
#include <duke/attributes/Attributes.hpp>
#include <duke/attributes/AttributeKeys.hpp>
#include <duke/imageio/PackedFrameDescription.hpp>
#include <duke/gl/Textures.hpp>
#include <duke/filesystem/MemoryMappedFile.hpp>
#include <duke/filesystem/FsUtils.hpp>
#include <duke/memory/Allocator.hpp>
#include <sstream>

using std::move;

namespace duke {

namespace {

AlignedMalloc alignedMalloc;

InputFrameOperationResult err(const std::string& error, InputFrameOperationResult& result) {
    result.error = error;
    return move(result);
}

InputFrameOperationResult loadImage(IImageReader *pRawReader, const Attributes& readOptions, const LoadCallback& callback, InputFrameOperationResult&& result) {
    std::unique_ptr<IImageReader> pReader(pRawReader);
    if (!pReader) return err("bad state : IImageReader==nullptr", result);
    if (pReader->hasError()) return err(pReader->getError(), result);
    RawPackedFrame& packedFrame = result.rawPackedFrame;
    if (!pReader->setup(readOptions, packedFrame)) return err(pReader->getError(), result);
    const void* pMapped = pReader->getMappedImageData();
    if (pMapped == nullptr) {
        packedFrame.pData = make_shared_memory<char>(packedFrame.description.dataSize, alignedMalloc);
        pReader->readImageDataTo(packedFrame.pData.get());
        if (pReader->hasError()) return err(pReader->getError(), result);
        callback(packedFrame, packedFrame.pData.get());
    } else {
        callback(packedFrame, pMapped);
    }
    result.status = IOOperationResult::SUCCESS;
    return move(result);
}

InputFrameOperationResult tryReader(const char* filename, const IIODescriptor *pDescriptor, const Attributes& readOptions, const LoadCallback& callback, InputFrameOperationResult&& result) {
    if (pDescriptor->supports(IIODescriptor::Capability::READER_READ_FROM_MEMORY)) {
        MemoryMappedFile file(filename);
        if (!file) {
            result.error = "unable to map file to memory";
            return move(result);
        }
        return loadImage(pDescriptor->getReaderFromMemory(file.pFileData, file.fileSize), readOptions, callback, move(result));
    } else {
        return loadImage(pDescriptor->getReaderFromFile(filename), readOptions, callback, move(result));
    }
}

InputFrameOperationResult load(const char* pFilename, const char *pExtension, const Attributes& readOptions, const LoadCallback& callback, InputFrameOperationResult&& result) {
    const auto &descriptors = IODescriptors::instance().findDescriptor(pExtension);
    if (descriptors.empty()) {
        result.error = "no reader available";
        return move(result);
    }
    for (const IIODescriptor *pDescriptor : descriptors) {
        result = tryReader(pFilename, pDescriptor, readOptions, callback, move(result));
        if (result) return move(result);
    }
    result.error = "no reader succeeded, last message was : '" + result.error + "'";
    return move(result);
}

}  // namespace

InputFrameOperationResult load(const Attributes& readOptions, const LoadCallback& callback, InputFrameOperationResult&& result) {
    const char* pFilename = result.attributes().findString(attribute::pDukeFilePathKey);
    if (!pFilename) {
        result.error = "no filename";
        return move(result);
    }
    const char* pExtension = fileExtension(pFilename);
    if (!pExtension) {
        result.error = "no extension";
        return move(result);
    }
    return load(pFilename, pExtension, readOptions, callback, move(result));
}

InputFrameOperationResult load(const char* pFilename, Texture& texture) {
    CHECK(pFilename);
    InputFrameOperationResult result;
    result.attributes().emplace_back(attribute::pDukeFilePathKey, pFilename);
    return load( { }, [&](RawPackedFrame& packedFrame, const void* pVolatileData) {
        const auto bound = texture.scope_bind_texture();
        texture.initialize(packedFrame.description,pVolatileData);
    }, std::move(result));
}

} /* namespace duke */
