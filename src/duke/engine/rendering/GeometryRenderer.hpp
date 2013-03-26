#pragma once

#include <duke/NonCopyable.hpp>
#include <duke/engine/rendering/MeshPool.hpp>
#include <duke/engine/rendering/ShaderPool.hpp>
#include <glm/glm.hpp>

namespace duke {

struct GeometryRenderer: public noncopyable {
	void drawRect(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) const;
	void drawLine(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) const;

	const ShaderPool shaderPool;
	const MeshPool meshPool;
};

} /* namespace duke */
