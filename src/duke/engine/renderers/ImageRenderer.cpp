/*
 * ImageRendere.cpp
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#include "ImageRenderer.h"
#include <duke/imageio/DukeIO.h>
#include <duke/imageio/Attributes.h>
#include <duke/engine/Context.h>
#include <duke/engine/ShaderPool.h>
#include <duke/gl/Mesh.hpp>
#include <duke/gl/Textures.h>

namespace duke {

static ShaderPool gProgramPool;

void render(const Mesh *pMesh, const ITexture& texture, const Attributes &attributes, const Context &context) {
	const auto &description = texture.description;
	bool redBlueSwapped = description.swapRedAndBlue;
	if (isInternalOptimizedFormatRedBlueSwapped(description.glPackFormat))
		redBlueSwapped = !redBlueSwapped;
	const ShaderDescription shaderDesc(description.swapEndianness, redBlueSwapped, description.glPackFormat == GL_RGB10_A2UI);
	const auto pProgram = gProgramPool.get(shaderDesc);
	pProgram->use();
	setTextureDimensions(pProgram->getUniformLocation("gImage"), description.width, description.height, attributes.getOrientation());
	const auto scopeBind = scope_bind(texture);
	glUniform2i(pProgram->getUniformLocation("gViewport"), context.viewport.dimension.x, context.viewport.dimension.y);
	glUniform1i(pProgram->getUniformLocation("gTextureSampler"), 0);
	glUniform2i(pProgram->getUniformLocation("gPan"), context.pan.x, context.pan.y);
	glUniform1i(pProgram->getUniformLocation("gZoom"), context.zoom);
	glUniform1f(pProgram->getUniformLocation("gExposure"), context.exposure);
	glUniform4i(pProgram->getUniformLocation("gShowChannel"), context.channels.x, context.channels.y, context.channels.z, context.channels.w);
	pMesh->draw();
	glCheckError();
}

} /* namespace duke */
