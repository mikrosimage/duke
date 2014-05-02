#include "Textures.hpp"
#include <duke/gl/GLUtils.hpp>
#include <duke/engine/ImageLoadUtils.hpp>

namespace duke {

void Texture::initialize(const FrameDescription &description, const GLvoid *pData) {
  const auto packFormat = description.glFormat;
  initialize(description, getAdaptedInternalFormat(packFormat), getPixelFormat(packFormat), getPixelType(packFormat),
             pData);
}

void Texture::initialize(const FrameDescription &description, GLint internalFormat, GLenum format, GLenum type,
                         const GLvoid *pData) {
  glCheckBound(target, id);
  //	printf("about to glTexImage2D, original internal %s, internal %s, pixel format %s, pixel type %s\n", //
  //			getInternalFormatString(description.glPackFormat), //
  //			getInternalFormatString(internalFormat), //
  //			getPixelFormatString(format), //
  //			getPixelTypeString(type));
  glTexImage2D(target, 0, internalFormat, description.width, description.height, 0, format, type, pData);
  glCheckError();
  this->description = description;
}

void Texture::update(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type,
                     const GLvoid *pData) {
  glCheckBound(target, id);
  glTexSubImage2D(target, 0, xoffset, yoffset, width, height, format, type, pData);
  glCheckError();
}

}  // namespace duke
