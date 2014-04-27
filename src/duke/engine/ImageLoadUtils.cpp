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

InputFrameOperationResult error(const std::string& error, InputFrameOperationResult& result) {
    result.error = error;
    result.status = IOOperationResult::FAILURE;
    return move(result);
}

InputFrameOperationResult tryReader(const char* filename, const IIODescriptor *pDescriptor, const attribute::Attributes& readOptions, const LoadCallback& callback, InputFrameOperationResult&& result) {
    std::unique_ptr<IImageReader> pReader;
    if (pDescriptor->supports(IIODescriptor::Capability::READER_READ_FROM_MEMORY)) {
        MemoryMappedFile file(filename);
        if (!file) return error("unable to map file to memory", result);
        pReader.reset(pDescriptor->getReaderFromMemory(readOptions, file.pFileData, file.fileSize));
    } else {
        pReader.reset(pDescriptor->getReaderFromFile(readOptions, filename));
    }
    return loadImage(pReader.get(), callback, move(result));
}

InputFrameOperationResult load(const char* pFilename, const char *pExtension, const attribute::Attributes& readOptions, const LoadCallback& callback, InputFrameOperationResult&& result) {
    const auto &descriptors = IODescriptors::instance().findDescriptor(pExtension);
    if (descriptors.empty()) return error("no reader available", result);
    for (const IIODescriptor *pDescriptor : descriptors) {
        result = tryReader(pFilename, pDescriptor, readOptions, callback, move(result));
        if (result) return move(result);
    }
    return error("no reader succeeded, last message was : '" + result.error + "'", result);
}

}  // namespace

InputFrameOperationResult loadImage(IImageReader *pReader, const LoadCallback& callback, InputFrameOperationResult&& result) {
    CHECK(pReader);
    if (pReader->hasError()) return error(pReader->getError(), result);
    RawPackedFrame& packedFrame = result.rawPackedFrame;
    if (!pReader->setup(packedFrame)) return error(pReader->getError(), result);
    const void* pMapped = pReader->getMappedImageData();
    if (pMapped) {
        callback(packedFrame, pMapped);
    } else {
        packedFrame.pData = make_shared_memory<char>(packedFrame.description.dataSize, alignedMalloc);
        pReader->readImageDataTo(packedFrame.pData.get());
        if (pReader->hasError()) return error(pReader->getError(), result);
        callback(packedFrame, packedFrame.pData.get());
    }
    result.status = IOOperationResult::SUCCESS;
    return move(result);
}

InputFrameOperationResult load(const attribute::Attributes& readOptions, const LoadCallback& callback, InputFrameOperationResult&& result) {
    const char* pFilename = attribute::getOrDie<attribute::File>(result.attributes());
    if (!pFilename) return error("no filename", result);
    const char* pExtension = fileExtension(pFilename);
    if (!pExtension) return error("no extension", result);
    return load(pFilename, pExtension, readOptions, callback, move(result));
}

InputFrameOperationResult load(const char* pFilename, Texture& texture) {
    CHECK(pFilename);
    InputFrameOperationResult result;
    attribute::set<attribute::File>(result.attributes(), pFilename);
    return load( { }, [&](RawPackedFrame& packedFrame, const void* pVolatileData) {
        const auto bound = texture.scope_bind_texture();
        texture.initialize(packedFrame.description,pVolatileData);
    }, std::move(result));
}

} /* namespace duke */
