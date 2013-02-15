/*
 * ImageRendere.cpp
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#include "ImageRenderer.h"
#include <duke/StringUtils.h>
#include <duke/imageio/DukeIO.h>
#include <duke/attributes/Attributes.h>
#include <duke/attributes/AttributeKeys.h>
#include <duke/engine/Context.h>
#include <duke/engine/rendering/ShaderPool.h>
#include <duke/gl/Mesh.hpp>
#include <duke/gl/Textures.h>

namespace duke {

static ShaderPool gProgramPool;

static ColorSpace resolveFromMetadata(const char* pColorspace) {
	if (pColorspace) {
		if (streq(pColorspace, "Linear"))
			return ColorSpace::Linear;
		if (streq(pColorspace, "sRGB") || streq(pColorspace, "GammaCorrected"))
			return ColorSpace::sRGB;
		if (streq(pColorspace, "KodakLog"))
			return ColorSpace::KodakLog;
		if (streq(pColorspace, "Rec709"))
			return ColorSpace::Rec709;
		if (streq(pColorspace, "AdobeRGB"))
			return ColorSpace::AdobeRGB;
	}
	return ColorSpace::Source;
}

static ColorSpace resolveFromExtension(const char* pFileExtension) {
	if (pFileExtension) {
		if (streq(pFileExtension, "dpx"))
			return ColorSpace::KodakLog;
	}
	printf("Unable to find default ColorSpace for extension '%s' assuming sRGB\n", pFileExtension);
	return ColorSpace::sRGB;
}

static ColorSpace resolve(const Attributes &attributes, ColorSpace original) {
	if (original != ColorSpace::Source)
		return original;
	original = resolveFromMetadata(attributes.findString(attribute::pOiioColospaceKey));
	if (original != ColorSpace::Source)
		return original;
	return resolveFromExtension(attributes.findString(attribute::pDukeFileExtensionKey));
}

void render(const Mesh *pMesh, const Texture& texture, const Attributes &attributes, const Context &context) {
	const auto &description = texture.description;
	bool redBlueSwapped = description.swapRedAndBlue;
	if (isInternalOptimizedFormatRedBlueSwapped(description.glPackFormat))
		redBlueSwapped = !redBlueSwapped;
	const ShaderDescription shaderDesc(description.swapEndianness, //
			redBlueSwapped, //
			description.glPackFormat == GL_RGB10_A2UI, //
			resolve(attributes, context.colorSpace));
	const auto pProgram = gProgramPool.get(shaderDesc);
	pProgram->use();
	setTextureDimensions(pProgram->getUniformLocation("gImage"), description.width, description.height, attributes.getOrientation());
	const auto scopeBind = texture.scope_bind_texture();
	glUniform2i(pProgram->getUniformLocation("gViewport"), context.viewport.dimension.x, context.viewport.dimension.y);
	glUniform1i(pProgram->getUniformLocation("gTextureSampler"), 0);
	glUniform2i(pProgram->getUniformLocation("gPan"), context.pan.x, context.pan.y);
	glUniform1i(pProgram->getUniformLocation("gZoom"), context.zoom);
	glUniform1f(pProgram->getUniformLocation("gExposure"), context.exposure);
	glUniform1f(pProgram->getUniformLocation("gGamma"), context.gamma);
	glUniform4i(pProgram->getUniformLocation("gShowChannel"), context.channels.x, context.channels.y, context.channels.z, context.channels.w);
	pMesh->draw();
	glCheckError();
}

} /* namespace duke */
