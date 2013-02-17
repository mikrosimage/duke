/*
 * ImageLoadUtils.cpp
 *
 *  Created on: Jan 19, 2013
 *      Author: Guillaume Chatelet
 */

#include "ImageLoadUtils.h"
#include <duke/imageio/DukeIO.h>
#include <duke/attributes/Attributes.h>
#include <duke/attributes/AttributeKeys.h>
#include <duke/imageio/PackedFrameDescription.h>
#include <duke/gl/Textures.h>
#include <duke/filesystem/MemoryMappedFile.h>
#include <duke/filesystem/FsUtils.h>
#include <duke/memory/Allocator.h>
#include <sstream>

namespace duke {

static AlignedMalloc alignedMalloc;

static std::string loadImage(IImageReader *pRawReader, const LoadCallback& callback) {
	std::unique_ptr<IImageReader> pReader(pRawReader);
	if (!pReader)
		return "bad state : IImageReader==nullptr";
	if (pReader->hasError())
		return pReader->getError();
	RawPackedFrame packedFrame = pReader->getRawPackedFrame();
	const void* pMapped = pReader->getMappedImageData();
	if (pMapped == nullptr) {
		packedFrame.pData = make_shared_memory<char>(packedFrame.description.dataSize, alignedMalloc);
		pReader->readImageDataTo(packedFrame.pData.get());
		if (pReader->hasError())
			return pReader->getError();
		callback(std::move(packedFrame), packedFrame.pData.get());
	} else {
		callback(std::move(packedFrame), pMapped);
	}
	return std::string();
}

static std::string tryReader(const char* filename, const IIODescriptor *pDescriptor, const LoadCallback& callback) {
	if (pDescriptor->supports(IIODescriptor::Capability::READER_READ_FROM_MEMORY)) {
		MemoryMappedFile file(filename);
		if (!file)
			return "unable to map file to memory";
		return loadImage(pDescriptor->getReaderFromMemory(file.pFileData, file.fileSize), callback);
	} else {
		return loadImage(pDescriptor->getReaderFromFile(filename), callback);
	}
}

static std::string load(const char* pFilename, const char *pExtension, const LoadCallback& callback) {
	const auto &descriptors = IODescriptors::instance().findDescriptor(pExtension);
	if (descriptors.empty())
		return "no reader available";
	for (const IIODescriptor *pDescriptor : descriptors) {
		std::string error = tryReader(pFilename, pDescriptor, callback);
		if (error.empty())
			return std::string();
	}
	return "no reader succeeded";
}

bool load(const char* pFilename, const char* pExtension, const LoadCallback& callback, std::string &error) {
	error = load(pFilename, pExtension, callback);
	if (error.empty())
		return true;
	return false;
}

bool load(const char* pFilename, Texture& texture, Attributes &attributes, std::string &error) {
	const char* pExtension = fileExtension(pFilename);
	if (!pExtension)
		return "no extension for file";
	const LoadCallback fCallback = [&](RawPackedFrame&& packedFrame, const void* pVolatileData) {
		attributes= std::move(packedFrame.attributes);
		attributes.emplace_back(attribute::pDukeFileExtensionKey,pExtension);
		const auto bound = texture.scope_bind_texture();
		texture.initialize(packedFrame.description,pVolatileData);
	};
	return load(pFilename, pExtension, fCallback, error);
}

} /* namespace duke */
