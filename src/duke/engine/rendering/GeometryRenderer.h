/*
 * GeometryRenderer.h
 *
 *  Created on: Feb 23, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef GEOMETRYRENDERER_H_
#define GEOMETRYRENDERER_H_

#include <glm/glm.hpp>

namespace duke {

void drawRect(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color);
void drawLine(const glm::ivec2 &viewport, const glm::ivec2 &dimensions, const glm::ivec2 &pan, const glm::vec4 &color);

} /* namespace duke */
#endif /* GEOMETRYRENDERER_H_ */
