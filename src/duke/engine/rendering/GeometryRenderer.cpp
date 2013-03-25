/*
 * GeometryRenderer.cpp
 *
 *  Created on: Feb 23, 2013
 *      Author: Guillaume Chatelet
 */

#include "GeometryRenderer.h"
#include <duke/gl/Mesh.h>
#include <duke/engine/Context.h>

namespace duke {

static void render(const SharedProgram& pProgram, const SharedMesh &pMesh, const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan,
		const glm::vec4 &color) {
	pProgram->use();
	glUniform2i(pProgram->getUniformLocation("gImage"), dimensions.x, dimensions.y);
	glUniform2i(pProgram->getUniformLocation("gViewport"), viewport.x, viewport.y);
	glUniform4f(pProgram->getUniformLocation("gSolidColor"), color.r, color.g, color.b, color.a);

	glUniform2i(pProgram->getUniformLocation("gPan"), pan.x, pan.y);
	glUniform1f(pProgram->getUniformLocation("gZoom"), 1);

	pMesh->draw();

	glCheckError();
}

void GeometryRenderer::drawRect(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) const {
	render(shaderPool.get(ShaderDescription::createSolidDesc()), meshPool.getSquare(), viewport, dimensions, pan, color);
}

void GeometryRenderer::drawLine(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) const {
	render(shaderPool.get(ShaderDescription::createSolidDesc()), meshPool.getLine(), viewport, dimensions, pan, color);
}
}
/* namespace duke */
