#pragma once

#include <duke/image/FrameDescriptionAndAttributes.hpp>
#include <duke/gl/Textures.hpp>
#include <duke/gl/GLUtils.hpp>

#include <memory>

namespace duke {

struct TexturePackedFrame : public FrameDescriptionAndAttributes {
  TexturePackedFrame(const PboPackedFrame &pbo, const std::shared_ptr<Texture> &pTexture)
      : FrameDescriptionAndAttributes(pbo), pTexture(pTexture) {
    auto pboBound = pbo.pPbo->scope_bind_buffer();
    auto textureBound = pTexture->scope_bind_texture();
    auto pixelFormat = getPixelFormat(description.glFormat);
    auto pixelType = getPixelType(description.glFormat);
    glTexSubImage2D(pTexture->target, 0, 0, 0, description.width, description.height, pixelFormat, pixelType, nullptr);
  }
  std::shared_ptr<Texture> pTexture;
};

} /* namespace duke */
