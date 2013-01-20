/*
 * ImageLoadUtils.cpp
 *
 *  Created on: Jan 19, 2013
 *      Author: Guillaume Chatelet
 */

#include "ImageLoadUtils.h"
#include <duke/imageio/DukeIO.h>
#include <duke/imageio/Attributes.h>
#include <duke/imageio/ImageDescription.h>
#include <duke/gl/Textures.h>
#include <duke/filesystem/MemoryMappedFile.h>
#include <duke/memory/alloc/Allocator.h>
#include <cstring>
#include <sstream>

namespace duke {

static AlignedMalloc alignedMalloc;

static std::string loadImage(IImageReader *pRawReader, const LoadCallback& callback) {
	std::unique_ptr<IImageReader> pReader(pRawReader);
	if (!pReader)
		return "bad state : IImageReader==nullptr";
	if (pReader->hasError())
		return pReader->getError();
	const auto &description = pReader->getDescription();
	const auto &attributes = pReader->getAttributes();
	const void* pMapped = pReader->getMappedImageData();
	if (pMapped == nullptr) {
		const auto pData = make_shared_memory<char>(description.dataSize, alignedMalloc);
		pReader->readImageDataTo(pData.get());
		if (pReader->hasError())
			return pReader->getError();
		callback(description, attributes, pData.get());
	} else {
		callback(description, attributes, pMapped);
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

static std::string load(const char* pFilename, const LoadCallback& callback) {
	const char* pDot = strrchr(pFilename, '.');
	if (!pDot)
		return "no extension for file";
	const auto &descriptors = IODescriptors::instance().findDescriptor(++pDot);
	if (descriptors.empty())
		return "no reader available";
	for (const IIODescriptor *pDescriptor : descriptors) {
		std::string error = tryReader(pFilename, pDescriptor, callback);
		if (error.empty())
			return std::string();
	}
	return "no reader succeeded in reading the file";
}

bool load(const char* pFilename, const LoadCallback& callback, std::string &error) {
	error = load(pFilename, callback);
	if (error.empty())
		return true;
	printf("error while reading %s : %s\n", pFilename, error.c_str());
	return false;
}

bool load(const char* pFilename, ITexture& texture, Attributes &attributes, std::string &error) {
	return load(pFilename, [&](const PackedFrameDescription &description, const Attributes &_attributes, const void* pData) {
		attributes= _attributes;
		const auto bound = scope_bind(texture);
		texture.initialize(description,pData);
	}, error);
}

} /* namespace duke */
