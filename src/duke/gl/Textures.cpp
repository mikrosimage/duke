#include "Textures.hpp"
#include <duke/gl/GlUtils.hpp>
#include <duke/imageio/ImageLoadUtils.hpp>

namespace duke {

void Texture::initialize(const ImageDescription &description, const GLvoid *pData) {
  const auto opengl_format = description.opengl_format;
  CHECK(opengl_format != -1) << "OpenGl format must be resolved at this point";
  initialize(description, getAdaptedInternalFormat(opengl_format), getPixelFormat(opengl_format),
             getPixelType(opengl_format), pData);
}

void Texture::initialize(const ImageDescription &description, GLint internalFormat, GLenum format, GLenum type,
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
