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
#include <duke/engine/rendering/ShaderFactory.h>
#include <duke/engine/rendering/ShaderPool.h>
#include <duke/gl/Mesh.hpp>
#include <duke/gl/Textures.h>

namespace duke {

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
	return ColorSpace::Auto;
}

static ColorSpace resolveFromExtension(const char* pFileExtension) {
	if (pFileExtension) {
		if (streq(pFileExtension, "dpx"))
			return ColorSpace::KodakLog;
		if (streq(pFileExtension, "png"))
			return ColorSpace::sRGB;
	}
	printf("Unable to find default ColorSpace for extension '%s' assuming sRGB\n", pFileExtension);
	return ColorSpace::sRGB;
}

static ColorSpace resolve(const Attributes &attributes, ColorSpace original) {
	if (original != ColorSpace::Auto)
		return original;
	original = resolveFromMetadata(attributes.findString(attribute::pOiioColospaceKey));
	if (original != ColorSpace::Auto)
		return original;
	return resolveFromExtension(attributes.findString(attribute::pDukeFileExtensionKey));
}

static inline float getAspectRatio(glm::vec2 dim) {
	return dim.x / dim.y;
}

static float getZoomValue(const Context &context) {
	switch (context.fitMode) {
	case FitMode::ACTUAL:
		return 1;
	case FitMode::FREE:
		return 1 + (0.1 * context.zoom);
	default:
		break;
	}
	if (!context.pCurrentImage)
		return 1;
	const auto viewportDim = glm::vec2(context.viewport.dimension);
	const auto viewportAspect = getAspectRatio(viewportDim);
	const auto &imageDescription = context.pCurrentImage->description;
	const auto imageDim = glm::vec2(imageDescription.width, imageDescription.height);
	const auto imageAspect = getAspectRatio(imageDim);
	switch (context.fitMode) {
	case FitMode::INNER:
		if (viewportAspect > imageAspect)
			return viewportDim.y / imageDim.y;
		return viewportDim.x / imageDim.x;
	case FitMode::OUTER:
		if (viewportAspect > imageAspect)
			return viewportDim.x / imageDim.x;
		return viewportDim.y / imageDim.y;
	default:
		throw std::runtime_error("invalid fit mode");
	}
}

void renderWithBoundTexture(const ShaderPool &shaderPool, const Mesh *pMesh, const Context &context) {
	const auto &description = context.pCurrentImage->description;
	bool redBlueSwapped = description.swapRedAndBlue;
	if (isInternalOptimizedFormatRedBlueSwapped(description.glPackFormat))
		redBlueSwapped = !redBlueSwapped;
	const ShaderDescription shaderDesc = ShaderDescription::createTextureDesc(description.swapEndianness, //
			redBlueSwapped, //
			description.glPackFormat == GL_RGB10_A2UI, //
			resolve(context.pCurrentImage->attributes, context.colorSpace));
	const auto pProgram = shaderPool.get(shaderDesc);
	pProgram->use();
	setTextureDimensions(pProgram->getUniformLocation("gImage"), description.width, description.height, context.pCurrentImage->attributes.getOrientation());
	glUniform2i(pProgram->getUniformLocation("gViewport"), context.viewport.dimension.x, context.viewport.dimension.y);
	glUniform1i(pProgram->getUniformLocation("gTextureSampler"), 0);
	glUniform2i(pProgram->getUniformLocation("gPan"), context.pan.x, context.pan.y);
	glUniform1f(pProgram->getUniformLocation("gExposure"), context.exposure);
	glUniform1f(pProgram->getUniformLocation("gGamma"), context.gamma);
	glUniform4i(pProgram->getUniformLocation("gShowChannel"), context.channels.x, context.channels.y, context.channels.z, context.channels.w);

	glUniform1f(pProgram->getUniformLocation("gZoom"), getZoomValue(context));
	pMesh->draw();
	glCheckError();
}

} /* namespace duke */
