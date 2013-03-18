/*
 * Viewport.h
 *
 *  Created on: Mar 3, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <glm/glm.hpp>

namespace duke {

struct Viewport {
	glm::ivec2 offset;
	glm::ivec2 dimension;

	Viewport() = default;
	Viewport(glm::ivec2 offset, glm::ivec2 dimension) :
	offset(offset), dimension(dimension) {
	}
};

}  // namespace duke
