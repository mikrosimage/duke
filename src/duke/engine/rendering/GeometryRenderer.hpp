#pragma once

#include <duke/base/NonCopyable.hpp>
#include <duke/engine/rendering/MeshPool.hpp>
#include <duke/engine/rendering/ShaderPool.hpp>
#include <glm/glm.hpp>

namespace duke {

struct GeometryRenderer: public noncopyable {
	void drawRect(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) const;
	void drawLine(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) const;

	ShaderPool shaderPool;
	MeshPool meshPool;
};

} /* namespace duke */
