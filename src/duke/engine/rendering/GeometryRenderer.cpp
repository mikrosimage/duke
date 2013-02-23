/*
 * GeometryRenderer.cpp
 *
 *  Created on: Feb 23, 2013
 *      Author: Guillaume Chatelet
 */

#include "GeometryRenderer.h"
#include <duke/gl/Mesh.hpp>
#include <duke/engine/Context.h>
#include <duke/engine/rendering/ShaderPool.h>

namespace duke {

static ShaderPool gProgramPool;

static void render(Mesh *pMesh, const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) {
	const auto pProgram = gProgramPool.get(ShaderDescription::createSolidDesc());
	pProgram->use();
	glUniform2i(pProgram->getUniformLocation("gImage"), dimensions.x, dimensions.y);
	glUniform2i(pProgram->getUniformLocation("gViewport"), viewport.x, viewport.y);
	glUniform4f(pProgram->getUniformLocation("gSolidColor"), color.r, color.g, color.b, color.a);

	glUniform2i(pProgram->getUniformLocation("gPan"), pan.x, pan.y);
	glUniform1f(pProgram->getUniformLocation("gZoom"), 1);

	pMesh->draw();

	glCheckError();
}
void drawRect(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) {
	static SharedMesh gUnitSquare = getSquare();
	render(gUnitSquare.get(), viewport, dimensions, pan, color);
}

void drawLine(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) {
	static SharedMesh gUnitLine = getLine();
	render(gUnitLine.get(), viewport, dimensions, pan, color);
}
}
/* namespace duke */
