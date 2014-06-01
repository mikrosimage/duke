#pragma once

#include "duke/image/ImageDescription.hpp"
#include "duke/gl/Textures.hpp"
#include "duke/gl/GlUtils.hpp"

#include <memory>

namespace duke {

struct TexturePackedFrame : public ImageDescription {
  TexturePackedFrame(const PboPackedFrame &pbo, const std::shared_ptr<Texture> &pTexture)
      : ImageDescription(pbo), pTexture(pTexture) {
    auto pboBound = pbo.pPbo->scope_bind_buffer();
    auto textureBound = pTexture->scope_bind_texture();
    CHECK(opengl_format != -1) << "OpenGl format must be resolved at this point";
    auto pixelFormat = getPixelFormat(opengl_format);
    auto pixelType = getPixelType(opengl_format);
    glTexSubImage2D(pTexture->target, 0, 0, 0, width, height, pixelFormat, pixelType, nullptr);
  }
  std::shared_ptr<Texture> pTexture;
};

} /* namespace duke */
