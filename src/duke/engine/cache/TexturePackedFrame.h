/*
 * TexturePackedFrame.h
 *
 *  Created on: Feb 16, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <duke/imageio/PackedFrameDescriptionAndAttributes.h>
#include <duke/gl/Textures.h>
#include <duke/gl/GLUtils.h>
#include <memory>

namespace duke {

struct TexturePackedFrame: public PackedFrameDescriptionAndAttributes {
	TexturePackedFrame(const PboPackedFrame &pbo, const std::shared_ptr<Texture> &pTexture) :
			PackedFrameDescriptionAndAttributes(pbo), pTexture(pTexture) {
		auto pboBound = pbo.pPbo->scope_bind_buffer();
		auto textureBound = pTexture->scope_bind_texture();
		auto pixelFormat = getPixelFormat(description.glPackFormat);
		auto pixelType = getPixelType(description.glPackFormat);
		glTexSubImage2D(pTexture->target, 0, 0, 0, description.width, description.height, pixelFormat, pixelType, nullptr);
	}
	std::shared_ptr<Texture> pTexture;
};

} /* namespace duke */
