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
		minFilter(GL_NEAREST), magFilter(GL_NEAREST), wrapMode(GL_CLAMP_TO_EDGE), textureType(type), m_pTextureBuffer(std::make_shared<TextureBuffer>(type)) {
}

GLint getInternalFormat(GLint format, GLint type) {
	switch (type) {
	case GL_UNSIGNED_BYTE:
		switch (format) {
		case GL_RGB:
		case GL_BGR:
			return GL_RGB8;
		case GL_RGBA:
		case GL_BGRA:
			return GL_RGBA8;
		}
		break;
	case GL_UNSIGNED_SHORT:
		switch (format) {
		case GL_RGB:
			return GL_RGB16;
		case GL_RGBA:
			return GL_RGBA16;
		}
		break;
	case GL_HALF_FLOAT:
		switch (format) {
		case GL_RGB:
			return GL_RGB16F;
		case GL_RGBA:
			return GL_RGBA16F;
		}
		break;
	case GL_FLOAT:
		switch (format) {
		case GL_RGB:
			return GL_RGB32F;
		case GL_RGBA:
			return GL_RGBA32F;
		}
		break;
	case GL_UNSIGNED_INT_10_10_10_2:
	case GL_UNSIGNED_INT_2_10_10_10_REV:
		return GL_RGB10;
	}
	throw std::runtime_error("Don't know how to map texture to OpenGL internal format");
}

void VolatileTexture::loadGlTexture(const void* pData) {
	glTexParameteri(textureType, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(textureType, GL_TEXTURE_WRAP_T, wrapMode);

	glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, magFilter);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(textureType, 0, //
			getInternalFormat(description.glFormat, description.glType), //
			description.width, description.height, 0, //
			description.glFormat, description.glType, pData);
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
	std::string error = "no reader";
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
	int orientation = 1;
	const auto pOrientation = attributes.find<int>("Orientation");
	if (pOrientation)
		orientation = pOrientation->getScalar<int>();
	float width = description.width;
	float height = description.height;
	switch (orientation) {
	case 0: //normal (top to bottom, left to right)
	case 1: //normal (top to bottom, left to right)
	case 2: //flipped horizontally (top to botom, right to left)
		height = -height;
		break;
	case 3: //rotate 180◦ (bottom to top, right to left)
	case 4: //flipped vertically (bottom to top, left to right)
		break;
	case 5: //transposed (left to right, top to bottom)
	case 6: //rotated 90◦ clockwise (right to left, top to bottom)
	case 7: //transverse (right to left, bottom to top)
	case 8: //rotated 90◦ counter-clockwise (left to right, bottom to top)
		throw std::runtime_error("unsupported orientation");
	}
	glUniform2f(dimensionUniformParameter, width, height);
	return ScopeBinder<TextureBuffer>(m_pTextureBuffer);
}
