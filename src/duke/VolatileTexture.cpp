/*
 * VolatileTexture.cpp
 *
 *  Created on: Dec 22, 2012
 *      Author: Guillaume Chatelet
 */

#include "VolatileTexture.h"
#include <duke/memory/alloc/Allocator.h>
#include <duke/io/MemoryMappedFile.h>

#include <iostream>

VolatileTexture::VolatileTexture(GLuint type) :
		minFilter(GL_NEAREST), magFilter(GL_NEAREST), wrapMode(GL_CLAMP_TO_EDGE), m_pTextureBuffer(std::make_shared<TextureBuffer>(GL_TEXTURE_RECTANGLE)) {
}

GLint getInternalFormat(GLint format) {
	switch (format) {
	case GL_BGR:
		return GL_RGB;
	case GL_BGRA:
		return GL_RGBA;
	default:
		return format;
	}
}

void VolatileTexture::loadGlTexture(const void* pData) {
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, wrapMode);

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, magFilter);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, getInternalFormat(description.glFormat), description.width, description.height, 0, description.glFormat, description.glType, pData);
	printf("loaded %ldx%ld", description.width, description.height);
	checkError();
}

static AlignedMalloc alignedMalloc;

std::string VolatileTexture::loadImage(std::unique_ptr<IImageReader> &&pReader) {
	if (!pReader) {
		return std::string("bad state : IImageReader==nullptr");
	}
	if (pReader->hasError()) {
		return pReader->getError();
	}
	description = pReader->getDescription();
	attributes = pReader->getAttributes();
	const void* pMapped = pReader->getMappedImageData();
	if (pMapped==nullptr) {
		const auto pData = make_shared_memory<char>(description.dataSize, alignedMalloc);
		pReader->readImageDataTo(pData.get());
		if (pReader->hasError()) {
			return pReader->getError();
		}
		loadGlTexture( pData.get());
	} else {
		loadGlTexture( pMapped);
	}
//	const Attribute * pAttr = attributes.find<int>("Orientation");
//	if(pAttr) {
//		switch(pAttr->getScalar<int>()) {
//			case 0:
//			description.height = -description.height;
//			break;
//		}
//	}
		return std::string();
	}

bool VolatileTexture::load(const char* filename, GLenum _minFilter, GLenum _magFilter, GLenum _wrapMode) {
	minFilter = _minFilter;
	magFilter = _magFilter;
	wrapMode = _wrapMode;
	const char* pDot = strrchr(filename, '.');
	if (!pDot)
		return false;
	ScopeBinder<TextureBuffer> scopeBind(m_pTextureBuffer);
	std::string error;
	for (const IIODescriptor *pDescriptor : IODescriptors::instance().findDescriptor(++pDot)) {
		std::unique_ptr<MemoryMappedFile> pMapped;
		std::unique_ptr<IImageReader> pReader;
		if (pDescriptor->supports(IIODescriptor::Capability::READER_READ_FROM_MEMORY)) {
			pMapped.reset(new MemoryMappedFile(filename)); // bad locality
			if (!(*pMapped)) {
				error = "unable to map file to memory";
				continue;
			}
			pReader.reset(pDescriptor->getReaderFromMemory(pMapped->pFileData, pMapped->fileSize));
		} else
			pReader.reset(pDescriptor->getReaderFromFile(filename));
		error = loadImage(std::move(pReader));
		if (error.empty())
			return true;
	}
	printf("error while reading %s : %s\n", filename, error.c_str());
	return false;
}

ScopeBinder<TextureBuffer> VolatileTexture::use(GLuint dimensionUniformParameter) const {
	if (attributes.find<int>("Orientation"))
		glUniform2f(dimensionUniformParameter, description.width, -float(description.height));
	else
		glUniform2f(dimensionUniformParameter, description.width, description.height);
	return ScopeBinder<TextureBuffer>(m_pTextureBuffer);
}
