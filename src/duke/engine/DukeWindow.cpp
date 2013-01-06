/*
 * Window.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: Guillaume Chatelet
 */

#include "DukeWindow.h"

#include <GL/gl.h>
#include <GL/glfw.h>

using namespace std;

namespace duke {

static void glViewport(const Viewport &viewport) {
	::glViewport(viewport.offset.x, viewport.offset.y, viewport.dimension.x, viewport.dimension.y);
}

DukeWindow::DukeWindow() :
		m_LeftButton(false) {
	mouseButtonCallback = bind(&DukeWindow::onMouseClick, this, placeholders::_1, placeholders::_2);
	mousePosCallback = bind(&DukeWindow::onMouseMove, this, placeholders::_1, placeholders::_2);
	windowResizeCallback = bind(&DukeWindow::onWindowResize, this, placeholders::_1, placeholders::_2);
	charCallback = bind(&DukeWindow::onKeyPressed, this, placeholders::_1, placeholders::_2);
}

void DukeWindow::onWindowResize(int width, int height) {
	::glViewport(0, 0, width, height);
	m_Dimension.x = width;
	m_Dimension.y = height;
}

void DukeWindow::onMouseMove(int x, int y) {
	m_MousePos.x = x;
	m_MousePos.y = m_Dimension.y - y;
}

void DukeWindow::onMouseClick(int buttonId, int buttonState) {
	if (buttonId == GLFW_MOUSE_BUTTON_LEFT)
		switch (buttonState) {
		case GLFW_PRESS:
			m_LeftButton = true;
			m_LeftDragOrigin = m_MousePos;
			break;
		case GLFW_RELEASE:
			m_LeftButton = false;
			m_LeftDragOrigin = glm::ivec2(0);
			break;
		}
}

void DukeWindow::onKeyPressed(int unicodeCodePoint, int keyState) {
	if (keyState == GLFW_PRESS)
		m_AllKeyStrokes.push_back(unicodeCodePoint);
}

const Viewport DukeWindow::useViewport(bool north, bool south, bool east, bool west) const {
	using namespace glm;
	assert(!(north && south));
	assert(!(east && west));
	const ivec2 halfDim(m_Dimension.x / 2, m_Dimension.y / 2);
	const ivec2 offset(east ? halfDim.x : 0, south ? halfDim.y : 0);
	const ivec2 dimension(east || west ? halfDim.x : m_Dimension.x, north || south ? halfDim.y : m_Dimension.y);
	const Viewport viewport(offset, dimension);
	glViewport(viewport);
	return viewport;
}

vector<int>& DukeWindow::getPendingKeys() {
	return m_AllKeyStrokes;
}

glm::ivec2 DukeWindow::getRelativeMousePos() {
	if (m_LeftButton)
		return m_MousePos - m_LeftDragOrigin;
	return glm::ivec2(0);
}

glm::ivec2 DukeWindow::getWindowMousePos() const {
	return m_MousePos;
}

glm::ivec2 DukeWindow::getViewportMousePos(const Viewport& viewport) const {
	return m_MousePos - viewport.offset;
}

}  // namespace duke
