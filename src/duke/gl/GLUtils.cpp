/*
 * GLUtils.cpp
 *
 *  Created on: Nov 28, 2012
 *      Author: Guillaume Chatelet
 */

#include "GLUtils.h"

#include <duke/gl/GL.h>
#include <stdexcept>

#include <fstream>
#include <sstream>
#include <vector>

const char* getInternalFormatString(GLint internalFormat) {
	switch (internalFormat) {
	case GL_DEPTH_COMPONENT:
		return "GL_DEPTH_COMPONENT";
	case GL_DEPTH_STENCIL:
		return "GL_DEPTH_STENCIL";
	case GL_RED:
		return "GL_RED";
	case GL_RG:
		return "GL_RG";
	case GL_RGB:
		return "GL_RGB";
	case GL_RGBA:
		return "GL_RGBA";
	case GL_R8:
		return "GL_R8";
	case GL_R8_SNORM:
		return "GL_R8_SNORM";
	case GL_R16:
		return "GL_R16";
	case GL_R16_SNORM:
		return "GL_R16_SNORM";
	case GL_RG8:
		return "GL_RG8";
	case GL_RG8_SNORM:
		return "GL_RG8_SNORM";
	case GL_RG16:
		return "GL_RG16";
	case GL_RG16_SNORM:
		return "GL_RG16_SNORM";
	case GL_R3_G3_B2:
		return "GL_R3_G3_B2";
	case GL_RGB4:
		return "GL_RGB4";
	case GL_RGB5:
		return "GL_RGB5";
	case GL_RGB8:
		return "GL_RGB8";
	case GL_RGB8_SNORM:
		return "GL_RGB8_SNORM";
	case GL_RGB10:
		return "GL_RGB10";
	case GL_RGB12:
		return "GL_RGB12";
	case GL_RGB16_SNORM:
		return "GL_RGB16_SNORM";
	case GL_RGBA2:
		return "GL_RGBA2";
	case GL_RGBA4:
		return "GL_RGBA4";
	case GL_RGB5_A1:
		return "GL_RGB5_A1";
	case GL_RGBA8:
		return "GL_RGBA8";
	case GL_RGBA8_SNORM:
		return "GL_RGBA8_SNORM";
	case GL_RGB10_A2:
		return "GL_RGB10_A2";
	case GL_RGB10_A2UI:
		return "GL_RGB10_A2UI";
	case GL_RGBA12:
		return "GL_RGBA12";
	case GL_RGBA16:
		return "GL_RGBA16";
	case GL_SRGB8:
		return "GL_SRGB8";
	case GL_SRGB8_ALPHA8:
		return "GL_SRGB8_ALPHA8";
	case GL_R16F:
		return "GL_R16F";
	case GL_RG16F:
		return "GL_RG16F";
	case GL_RGB16F:
		return "GL_RGB16F";
	case GL_RGBA16F:
		return "GL_RGBA16F";
	case GL_R32F:
		return "GL_R32F";
	case GL_RG32F:
		return "GL_RG32F";
	case GL_RGB32F:
		return "GL_RGB32F";
	case GL_RGBA32F:
		return "GL_RGBA32F";
	case GL_R11F_G11F_B10F:
		return "GL_R11F_G11F_B10F";
	case GL_RGB9_E5:
		return "GL_RGB9_E5";
	case GL_R8I:
		return "GL_R8I";
	case GL_R8UI:
		return "GL_R8UI";
	case GL_R16I:
		return "GL_R16I";
	case GL_R16UI:
		return "GL_R16UI";
	case GL_R32I:
		return "GL_R32I";
	case GL_R32UI:
		return "GL_R32UI";
	case GL_RG8I:
		return "GL_RG8I";
	case GL_RG8UI:
		return "GL_RG8UI";
	case GL_RG16I:
		return "GL_RG16I";
	case GL_RG16UI:
		return "GL_RG16UI";
	case GL_RG32I:
		return "GL_RG32I";
	case GL_RG32UI:
		return "GL_RG32UI";
	case GL_RGB8I:
		return "GL_RGB8I";
	case GL_RGB8UI:
		return "GL_RGB8UI";
	case GL_RGB16I:
		return "GL_RGB16I";
	case GL_RGB16UI:
		return "GL_RGB16UI";
	case GL_RGB32I:
		return "GL_RGB32I";
	case GL_RGB32UI:
		return "GL_RGB32UI";
	case GL_RGBA8I:
		return "GL_RGBA8I";
	case GL_RGBA8UI:
		return "GL_RGBA8UI";
	case GL_RGBA16I:
		return "GL_RGBA16I";
	case GL_RGBA16UI:
		return "GL_RGBA16UI";
	case GL_RGBA32I:
		return "GL_RGBA32I";
	case GL_RGBA32UI:
		return "GL_RGBA32UI";
	case GL_COMPRESSED_RED:
		return "GL_COMPRESSED_RED";
	case GL_COMPRESSED_RG:
		return "GL_COMPRESSED_RG";
	case GL_COMPRESSED_RGB:
		return "GL_COMPRESSED_RGB";
	case GL_COMPRESSED_RGBA:
		return "GL_COMPRESSED_RGBA";
	case GL_COMPRESSED_SRGB:
		return "GL_COMPRESSED_SRGB";
	case GL_COMPRESSED_SRGB_ALPHA:
		return "GL_COMPRESSED_SRGB_ALPHA";
	case GL_COMPRESSED_RED_RGTC1:
		return "GL_COMPRESSED_RED_RGTC1";
	case GL_COMPRESSED_SIGNED_RED_RGTC1:
		return "GL_COMPRESSED_SIGNED_RED_RGTC1";
	case GL_COMPRESSED_RG_RGTC2:
		return "GL_COMPRESSED_RG_RGTC2";
	case GL_COMPRESSED_SIGNED_RG_RGTC2:
		return "GL_COMPRESSED_SIGNED_RG_RGTC2";
//	case GL_COMPRESSED_RGBA_BPTC_UNORM:
//		return "GL_COMPRESSED_RGBA_BPTC_UNORM";
//	case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
//		return "GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM";
//	case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
//		return "GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT";
//	case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
//		return "GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT";
	}
	return "Unknown";
}

const char* getPixelFormatString(unsigned int pixelFormat) {
	switch (pixelFormat) {
	case GL_RED:
		return "GL_RED";
	case GL_RG:
		return "GL_RG";
	case GL_RGB:
		return "GL_RGB";
	case GL_BGR:
		return "GL_BGR";
	case GL_RGBA:
		return "GL_RGBA";
	case GL_BGRA:
		return "GL_BGRA";
	case GL_RED_INTEGER:
		return "GL_RED_INTEGER";
	case GL_RG_INTEGER:
		return "GL_RG_INTEGER";
	case GL_RGB_INTEGER:
		return "GL_RGB_INTEGER";
	case GL_BGR_INTEGER:
		return "GL_BGR_INTEGER";
	case GL_RGBA_INTEGER:
		return "GL_RGBA_INTEGER";
	case GL_BGRA_INTEGER:
		return "GL_BGRA_INTEGER";
	}
	return "Unknown";
}

const char* getPixelTypeString(unsigned int pixelType) {
	switch (pixelType) {
	case GL_UNSIGNED_BYTE:
		return "GL_UNSIGNED_BYTE";
	case GL_BYTE:
		return "GL_BYTE";
	case GL_UNSIGNED_SHORT:
		return "GL_UNSIGNED_SHORT";
	case GL_SHORT:
		return "GL_SHORT";
	case GL_UNSIGNED_INT:
		return "GL_UNSIGNED_INT";
	case GL_INT:
		return "GL_INT";
	case GL_HALF_FLOAT:
		return "GL_HALF_FLOAT";
	case GL_FLOAT:
		return "GL_FLOAT";
	case GL_UNSIGNED_BYTE_3_3_2:
		return "GL_UNSIGNED_BYTE_3_3_2";
	case GL_UNSIGNED_BYTE_2_3_3_REV:
		return "GL_UNSIGNED_BYTE_2_3_3_REV";
	case GL_UNSIGNED_SHORT_5_6_5:
		return "GL_UNSIGNED_SHORT_5_6_5";
	case GL_UNSIGNED_SHORT_5_6_5_REV:
		return "GL_UNSIGNED_SHORT_5_6_5_REV";
	case GL_UNSIGNED_SHORT_4_4_4_4:
		return "GL_UNSIGNED_SHORT_4_4_4_4";
	case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		return "GL_UNSIGNED_SHORT_4_4_4_4_REV";
	case GL_UNSIGNED_SHORT_5_5_5_1:
		return "GL_UNSIGNED_SHORT_5_5_5_1";
	case GL_UNSIGNED_SHORT_1_5_5_5_REV:
		return "GL_UNSIGNED_SHORT_1_5_5_5_REV";
	case GL_UNSIGNED_INT_8_8_8_8:
		return "GL_UNSIGNED_INT_8_8_8_8";
	case GL_UNSIGNED_INT_8_8_8_8_REV:
		return "GL_UNSIGNED_INT_8_8_8_8_REV";
	case GL_UNSIGNED_INT_10_10_10_2:
		return "GL_UNSIGNED_INT_10_10_10_2";
	case GL_UNSIGNED_INT_2_10_10_10_REV:
		return "GL_UNSIGNED_INT_2_10_10_10_REV";
	}
	return "Unknown";
}

#ifndef NDEBUG
static const char* getErrorString(unsigned error) {
	switch (error) {
	case GL_INVALID_ENUM:
		return "Invalid enum";
	case GL_INVALID_VALUE:
		return "Invalid value";
	case GL_INVALID_OPERATION:
		return "Invalid operation";
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "Invalid framebuffer operation";
	case GL_OUT_OF_MEMORY:
		return "Out of memory";
		//case GL_STACK_UNDERFLOW:
		//	throw std::runtime_error("OpenGL : Stack underflow");
		//case GL_STACK_OVERFLOW:
		//	throw std::runtime_error("OpenGL : Stack overflow");
	}
	return "Unknown error";
}
#endif

void glCheckError() {
#ifndef NDEBUG
	std::vector<unsigned> errors;
	unsigned error = GL_NO_ERROR;
	for (; (error = glGetError()) != GL_NO_ERROR;)
		errors.push_back(error);
	if (errors.empty())
		return;
	std::ostringstream oss;
	oss << "OpenGL errors :\n";
	for (const unsigned error : errors)
		oss << " - " << getErrorString(error) << '\n';
	throw std::runtime_error(oss.str());
#endif
}

static GLuint getBindParameter(GLuint targetType) {
	switch (targetType) {
	case GL_TEXTURE_2D:
		return GL_TEXTURE_BINDING_2D;
	case GL_TEXTURE_RECTANGLE:
		return GL_TEXTURE_BINDING_RECTANGLE;
	case GL_ARRAY_BUFFER:
		return GL_ARRAY_BUFFER_BINDING;
	case GL_ELEMENT_ARRAY_BUFFER:
		return GL_ELEMENT_ARRAY_BUFFER_BINDING;
	case GL_PIXEL_UNPACK_BUFFER:
		return GL_PIXEL_UNPACK_BUFFER_BINDING;
	case GL_PIXEL_PACK_BUFFER:
		return GL_PIXEL_PACK_BUFFER_BINDING;
	};
	throw std::runtime_error("unsupported targetType");
}

void glCheckBound(GLuint targetType, GLuint id) {
	GLint current;
	glGetIntegerv(getBindParameter(targetType), &current);
	if (GLuint(current) != id)
		throw std::runtime_error("Trying to operate on unbound GlObject");
}

void checkShaderError(GLuint shaderId, const char* pSource) {
	GLint success;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (success)
		return;
	GLchar InfoLog[1024];
	glGetShaderInfoLog(shaderId, sizeof(InfoLog), NULL, InfoLog);
	char error[1024];
	snprintf(error, sizeof(error), "OpenGL : error compiling shader type :\n%s\nsource :'%s'", InfoLog, pSource);
	throw std::runtime_error(error);
}

void checkProgramError(unsigned int programId) {
	GLint success;
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (success)
		return;
	GLchar InfoLog[1024];
	glGetProgramInfoLog(programId, sizeof(InfoLog), NULL, InfoLog);
	char error[1024];
	snprintf(error, sizeof(error), "OpenGL : linking shader program :\n%s\n", InfoLog);
	throw std::runtime_error(error);
}

GLenum getPixelFormat(GLint internalFormat) {
	switch (internalFormat) {
	case GL_R8:
		return GL_RED;
	case GL_RGB8:
	case GL_RGB16F:
		return GL_RGB;
	case GL_RGBA8:
	case GL_RGBA16F:
		return GL_RGBA;
	case GL_RGB10_A2UI:
		return GL_RGBA_INTEGER;
	default:
		std::ostringstream oss;
		oss << "Don't know how to convert internal image format ";
		oss << getInternalFormatString(internalFormat) << " to pixel format";
		throw std::runtime_error(oss.str());
	}
}

bool isInternalOptimizedFormatRedBlueSwapped(int internalFormat) {
	switch (internalFormat) {
	case GL_R8:
	case GL_RGB8:
	case GL_RGBA8:
	case GL_RGB10_A2UI:
	case GL_RGB16F:
	case GL_RGBA16F:
		return false;
	default:
		return true;
	}
}

GLint getAdaptedInternalFormat(GLint internalFormat) {
	return internalFormat == GL_RGB10_A2UI ? GL_RGBA8UI : internalFormat;
}

GLenum getPixelType(GLint internalFormat) {
	switch (internalFormat) {
	case GL_R8:
	case GL_RGB8:
		return GL_UNSIGNED_BYTE;
	case GL_RGB10_A2UI:
	case GL_RGBA8:
		return GL_UNSIGNED_INT_8_8_8_8_REV;
	case GL_RGBA16F:
	case GL_RGB16F:
		return GL_HALF_FLOAT;
	default:
		std::ostringstream oss;
		oss << "Don't know how to convert internal image format ";
		oss << getInternalFormatString(internalFormat) << " to pixel type";
		throw std::runtime_error(oss.str());
	}
}

size_t getChannelCount(GLenum pixel_format) {
	switch (pixel_format) {
	case GL_RGBA:
	case GL_BGRA:
		return 4;
	case GL_RGB:
	case GL_BGR:
		return 3;
	case GL_RED:
		return 3;
	default:
		throw std::runtime_error("channel count not implemented");
	}
}

size_t getBytePerChannel(GLenum pixel_type) {
	switch (pixel_type) {
	case GL_UNSIGNED_INT_8_8_8_8:
	case GL_UNSIGNED_INT_8_8_8_8_REV:
	case GL_UNSIGNED_BYTE:
		return 1;
	case GL_UNSIGNED_SHORT:
	case GL_HALF_FLOAT:
		return 2;
	case GL_FLOAT:
	case GL_INT:
		return 4;
	default:
		throw std::runtime_error("byte per channel not implemented");
	}
}

size_t getBytePerPixels(GLenum pixel_format, GLenum pixel_type) {
	return getChannelCount(pixel_format) * getBytePerChannel(pixel_type);
}

std::string slurpFile(const char* pFilename) {
	std::ifstream in(pFilename);
	if (!in)
		throw std::ios_base::failure(std::string("unable to load file : ") + pFilename);
	std::stringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

std::vector<unsigned char> slurpBinaryFile(const char* pFilename) {
	std::ifstream in(pFilename, std::ios::in | std::ios::binary | std::ios::ate);
	if (!in)
		throw std::ios_base::failure(std::string("unable to load file : ") + pFilename);
	const size_t fileSize = in.tellg();
	std::vector<unsigned char> result;
	result.resize(fileSize);
	in.seekg(0, std::ios::beg);
	in.read(reinterpret_cast<char*>(result.data()), fileSize);
	return result;
}

void setTextureDimensions(GLuint dimensionUniformParameter, size_t uwidth, size_t uheight, int orientation) {
	float width = uwidth;
	float height = uheight;
	switch (orientation) {
	case 0: //normal (top to bottom, left to right)
	case 1: //normal (top to bottom, left to right)
	case 2: //flipped horizontally (top to bottom, right to left)
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
}
