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
#include <vector>
#include <cassert>

namespace duke {

struct Viewport {
	glm::ivec2 offset;
	glm::ivec2 dimension; Viewport() = default;
	Viewport(glm::ivec2 offset, glm::ivec2 dimension) :
	offset(offset), dimension(dimension) {
	}
};

struct DukeWindow: public DukeGLFWWindow {
	DukeWindow(GLFWwindow *pWindow);
	//setter
	void onKey(int key, int action);
	void onChar(int unicodeCodePoint);
	void onWindowResize(int width, int height);
	void onMouseMove(int x, int y);
	void onMouseClick(int buttonId, int buttonState);
	void onScroll(double x, double y);

	void setScroll(glm::vec2 scroll);
	void setPan(glm::ivec2 pan);

	// getter
	const Viewport useViewport(bool north, bool south, bool east, bool west) const;
	glm::vec2 getScrollPos() const;
	glm::ivec2 getPanPos() const;
	glm::ivec2 getRelativeMousePos() const;
	glm::ivec2 getViewportMousePos(const Viewport& viewport) const;
	std::vector<int>& getPendingChars();
	std::vector<int>& getPendingKeys();

	// GLFW functions
	void makeContextCurrent();
	int glfwGetKey(int key);
	int glfwGetWindowParam(int param);
	void glfwSwapBuffers();
private:
	bool m_LeftButton;
	glm::ivec2 m_MousePos;
	glm::ivec2 m_MouseDragStartPos;
	glm::ivec2 m_Pan;
	glm::ivec2 m_Dimension;
	glm::vec2 m_Scroll;
	std::vector<int> m_CharStrokes;
	std::vector<int> m_KeyStrokes;
};

}  // namespace duke

#endif /* WINDOW_H_ */
