/*
 * ImageRenderer.cpp
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#include "ImageRenderer.h"
#include <duke/engine/Context.h>
#include <duke/engine/LoadableTexture.h>

#include <GL/gl.h>

namespace duke {

AbstractRenderer::AbstractRenderer(SharedVertexShader vs, SharedFragmentShader fs) :
		m_Program(vs, fs), gViewport(m_Program.getUniformLocation("gViewport")), //
		gImage(m_Program.getUniformLocation("gImage")), //
		gPan(m_Program.getUniformLocation("gPan")), //
		gTextureSampler(m_Program.getUniformLocation("rectangleImageSampler")), //
		m_pMesh(getSquare()) {
}

AbstractRenderer::~AbstractRenderer() {
}

ImageRenderer::ImageRenderer() :
		AbstractRenderer(loadVertexShader("shader/vertex_pan_scan_texture.glsl"), loadFragmentShader("shader/basic_grading.fglsl")), //
		gZoom(m_Program.getUniformLocation("gZoom")), //
		gShowChannel(m_Program.getUniformLocation("gShowChannel")), //
		gExposure(m_Program.getUniformLocation("gExposure")) {
}

void ImageRenderer::draw(const duke::Context &context, const LoadableTexture& texture) const {
	m_Program.use();
	glUniform2i(gViewport, context.viewport.dimension.x, context.viewport.dimension.y);
	glUniform1i(gTextureSampler, 0);
	const auto scopeBind = texture.use(gImage);
	glUniform2i(gPan, context.pan.x, context.pan.y);
	glUniform1i(gZoom, context.zoom);
	glUniform1f(gExposure, context.exposure);
	glUniform4i(gShowChannel, context.channels.x, context.channels.y, context.channels.z, context.channels.w);
	m_pMesh->draw();
	checkError();
}

} /* namespace duke */
