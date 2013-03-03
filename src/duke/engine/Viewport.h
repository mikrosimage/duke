/*
 * Viewport.h
 *
 *  Created on: Mar 3, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef VIEWPORT_H_
#define VIEWPORT_H_

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

#endif /* VIEWPORT_H_ */
