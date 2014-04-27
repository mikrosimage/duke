#include "ImageRenderer.hpp"
#include <duke/imageio/DukeIO.hpp>
#include <duke/attributes/Attributes.hpp>
#include <duke/attributes/AttributeKeys.hpp>
#include <duke/engine/Context.hpp>
#include <duke/engine/rendering/ShaderFactory.hpp>
#include <duke/engine/rendering/ShaderPool.hpp>
#include <duke/engine/rendering/ShaderConstants.hpp>
#include <duke/filesystem/FsUtils.hpp>
#include <duke/gl/Mesh.hpp>
#include <duke/gl/Textures.hpp>
#include <duke/engine/ColorSpace.hpp>

namespace duke {

namespace {

ColorSpace resolve(const attribute::Attributes &attributes, ColorSpace original) {
	if (original != ColorSpace::Auto)
		return original;
	original = resolveFromName(attribute::getWithDefault<attribute::OiioColorspace>(attributes));
	if (original != ColorSpace::Auto)
		return original;
    return resolveFromExtension(fileExtension(attribute::getOrDie<attribute::File>(attributes)));
}

inline float getAspectRatio(glm::vec2 dim) {
	return dim.x / dim.y;
}

}  // namespace

float getZoomValue(const Context &context) {
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

namespace {

bool isGreyscale(size_t glPackFormat) {
	switch (glPackFormat) {
	case GL_R8:
	case GL_R16:
	case GL_R32UI:
	case GL_R32I:
	case GL_R16F:
	case GL_R32F:
		return true;
	default:
		return false;
	}
}

}  // namespace

void renderWithBoundTexture(const ShaderPool &shaderPool, const Mesh *pMesh, const Context &context) {
	const auto &description = context.pCurrentImage->description;
	bool redBlueSwapped = description.swapRedAndBlue;
	if (isInternalOptimizedFormatRedBlueSwapped(description.glPackFormat))
		redBlueSwapped = !redBlueSwapped;

	const auto& currentImageAttributes = context.pCurrentImage->attributes;
	const auto inputColorSpace = resolve(currentImageAttributes, context.fileColorSpace);

	const ShaderDescription shaderDesc = ShaderDescription::createTextureDesc( //
			isGreyscale(description.glPackFormat), //
			description.swapEndianness, //
			redBlueSwapped, //
			description.glPackFormat == GL_RGB10_A2UI, //
			inputColorSpace,
			context.screenColorSpace);
	const auto pProgram = shaderPool.get(shaderDesc);
    const auto pair = getTextureDimensions(description.width,
                                           description.height,
                                           attribute::getWithDefault<attribute::DpxImageOrientation>(currentImageAttributes));
	pProgram->use();
	pProgram->glUniform2i(shader::gImage, pair.first, pair.second);
	pProgram->glUniform2i(shader::gViewport, context.viewport.dimension.x, context.viewport.dimension.y);
	pProgram->glUniform1i(shader::gTextureSampler, 0);
	pProgram->glUniform2i(shader::gPan, context.pan.x, context.pan.y);
	pProgram->glUniform1f(shader::gExposure, context.exposure);
	pProgram->glUniform1f(shader::gGamma, context.gamma);
	pProgram->glUniform4i(shader::gShowChannel, context.channels.x, context.channels.y, context.channels.z, context.channels.w);

	pProgram->glUniform1f(shader::gZoom, context.zoom);
	pMesh->draw();
	glCheckError();
}

} /* namespace duke */
