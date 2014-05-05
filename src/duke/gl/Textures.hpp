#pragma once

#include <duke/gl/GlObjects.hpp>
#include <duke/image/FrameDescription.hpp>

namespace duke {

struct Texture : public gl::GlTextureRectangle {
  void initialize(const FrameDescription &description, const GLvoid *pData = nullptr);
  void initialize(const FrameDescription &description, GLint internalFormat, GLenum format, GLenum type,
                  const GLvoid *pData);
  void update(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type,
              const GLvoid *pData);

  FrameDescription description;
};

}  // namespace duke
