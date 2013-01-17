/*
 * VolatileTexture.cpp
 *
 *  Created on: Dec 22, 2012
 *      Author: Guillaume Chatelet
 */

#include "LoadableTexture.h"
#include <duke/memory/alloc/Allocator.h>
#include <duke/filesystem/MemoryMappedFile.h>

#include <iostream>

LoadableTexture::LoadableTexture() :
		minFilter(GL_NEAREST), magFilter(GL_NEAREST), wrapMode(GL_CLAMP_TO_EDGE), m_pTextureBuffer(std::make_shared<TextureRectangle>()) {
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

void LoadableTexture::loadGlTexture(const void* pData) {
	const auto textureType = m_pTextureBuffer->TargetType;
	glTexParameteri(textureType, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(textureType, GL_TEXTURE_WRAP_T, wrapMode);

	glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, magFilter);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	m_pTextureBuffer->texImage2D(0, //
			getInternalFormat(description.glFormat, description.glType), //
			description.width, description.height, 0, //
			description.glFormat, description.glType, pData);
	glCheckError();
}

static AlignedMalloc alignedMalloc;

std::string LoadableTexture::loadImage(IImageReader *pRawReader) {
	std::unique_ptr<IImageReader> pReader(pRawReader);
	if (!pReader)
		return "bad state : IImageReader==nullptr";
	if (pReader->hasError())
		return pReader->getError();
	description = pReader->getDescription();
	attributes = pReader->getAttributes();
	const void* pMapped = pReader->getMappedImageData();
	if (pMapped == nullptr) {
		const auto pData = make_shared_memory<char>(description.dataSize, alignedMalloc);
		pReader->readImageDataTo(pData.get());
		if (pReader->hasError())
			return pReader->getError();
		loadGlTexture(pData.get());
	} else {
		loadGlTexture(pMapped);
	}
	return std::string();
}

std::string LoadableTexture::tryReader(const char* filename, const IIODescriptor *pDescriptor) {
	if (pDescriptor->supports(IIODescriptor::Capability::READER_READ_FROM_MEMORY)) {
		MemoryMappedFile file(filename);
		if (!file)
			return "unable to map file to memory";
		return loadImage(pDescriptor->getReaderFromMemory(file.pFileData, file.fileSize));
	} else {
		return loadImage(pDescriptor->getReaderFromFile(filename));
	}
}

bool LoadableTexture::load(const char* filename, GLenum _minFilter, GLenum _magFilter, GLenum _wrapMode) {
	minFilter = _minFilter;
	magFilter = _magFilter;
	wrapMode = _wrapMode;
	const char* pDot = strrchr(filename, '.');
	if (!pDot)
		return false;
	const auto bound = scope_bind(*m_pTextureBuffer);
	std::string error = "no reader available";
	for (const IIODescriptor *pDescriptor : IODescriptors::instance().findDescriptor(++pDot))
		if ((error = tryReader(filename, pDescriptor)).empty())
			return true;
	printf("error while reading %s : %s\n", filename, error.c_str());
	return false;
}

Binder<TextureRectangle> LoadableTexture::use(GLuint dimensionUniformParameter) const {
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
	glUniform2i(dimensionUniformParameter, width, height);
	return scope_bind(*m_pTextureBuffer);
}
