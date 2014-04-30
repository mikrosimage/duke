#pragma once

#include <duke/gl/GlObjects.hpp>
#include <duke/imageio/PackedFrameDescription.hpp>

namespace duke {

struct Texture : public gl::GlTextureRectangle {
  void initialize(const PackedFrameDescription &description, const GLvoid *pData = nullptr);
  void initialize(const PackedFrameDescription &description, GLint internalFormat, GLenum format, GLenum type,
                  const GLvoid *pData);
  void update(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type,
              const GLvoid *pData);

  PackedFrameDescription description;
};

}  // namespace duke
