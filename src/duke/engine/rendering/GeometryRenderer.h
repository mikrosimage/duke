/*
 * GeometryRenderer.h
 *
 *  Created on: Feb 23, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef GEOMETRYRENDERER_H_
#define GEOMETRYRENDERER_H_

#include <duke/NonCopyable.h>
#include <duke/engine/rendering/MeshPool.h>
#include <duke/engine/rendering/ShaderPool.h>
#include <glm/glm.hpp>

namespace duke {

struct GeometryRenderer: public noncopyable {
	void drawRect(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) const;
	void drawLine(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color) const;

	const ShaderPool shaderPool;
	const MeshPool meshPool;
};

} /* namespace duke */
#endif /* GEOMETRYRENDERER_H_ */
