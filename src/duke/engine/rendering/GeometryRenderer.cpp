#include "GeometryRenderer.hpp"
#include <duke/gl/Mesh.hpp>
#include <duke/engine/Context.hpp>
#include <duke/engine/rendering/ShaderConstants.hpp>

namespace duke {

namespace {

void render(const SharedProgram& pProgram, const SharedMesh &pMesh, const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan,
		const glm::vec4 &color) {
	pProgram->use();
	pProgram->glUniform2i(shader::gImage, dimensions.x, dimensions.y);
	pProgram->glUniform2i(shader::gViewport, viewport.x, viewport.y);
	pProgram->glUniform4f(shader::gSolidColor, color.r, color.g, color.b, color.a);

	pProgram->glUniform2i(shader::gPan, pan.x, pan.y);
	pProgram->glUniform1f(shader::gZoom, 1);

	pMesh->draw();

	glCheckError();
}

}  // namespace

void GeometryRenderer::drawRect(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) const {
	render(shaderPool.get(ShaderDescription::createSolidDesc()), meshPool.getSquare(), viewport, dimensions, pan, color);
}

void GeometryRenderer::drawLine(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color, bool isPlaying) const {
	render(shaderPool.get(ShaderDescription::createSolidDesc()), meshPool.getLine(), viewport, dimensions, pan, color, isPlaying);
}
}
/* namespace duke */
