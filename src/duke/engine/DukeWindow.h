/*
 * Window.h
 *
 *  Created on: Nov 30, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <duke/NonCopyable.h>
#include <duke/gl/GlFwApp.h>

#include <glm/glm.hpp>
#include <set>
#include <cassert>

namespace duke {

struct Viewport {
	glm::ivec2 offset;
	glm::ivec2 dimension;
	Viewport() = default;
	Viewport(glm::ivec2 offset, glm::ivec2 dimension) :
			offset(offset), dimension(dimension) {
	}
};

struct DukeWindow: public NonCopyable, public GlFwApp {
	DukeWindow();
	const Viewport useViewport(bool north, bool south, bool east, bool west) const;
	glm::ivec2 getRelativeMousePos();
	glm::ivec2 getWindowMousePos() const;
	glm::ivec2 getViewportMousePos(const Viewport& viewport) const;
	std::set<int>& getPendingKeys();
private:
	void onKeyPressed(int unicodeCodePoint, int keyState);
	void onWindowResize(int width, int height);
	void onMouseMove(int x, int y);
	void onMouseClick(int buttonId, int buttonState);
	bool m_LeftButton;
	glm::ivec2 m_LeftDragOrigin;
	glm::ivec2 m_MousePos;
	glm::ivec2 m_Dimension;
	std::set<int> m_AllKeyStrokes;
};

}  // namespace duke

#endif /* WINDOW_H_ */
