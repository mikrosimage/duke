/*
 * GlApp.h
 *
 *  Created on: Nov 25, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef GLAPP_H_
#define GLAPP_H_

#include <duke/NonCopyable.h>

#include <functional>

struct GLFWwindow;
struct GLFWmonitor;

namespace duke {

class DukeGLFWWindow: public noncopyable {
public:
	DukeGLFWWindow(GLFWwindow *pWindow);
	virtual ~DukeGLFWWindow();

	void registerCallbacks();
	GLFWwindow *getHandle();

	std::function<int()> windowCloseCallback;
	std::function<void(int)> windowFocusCallback;
	std::function<void(int width, int height)> windowResizeCallback;
	std::function<void()> windowRefreshFunCallback;
	std::function<void(int buttonId, int buttonState)> mouseButtonCallback;
	std::function<void(int x, int y)> mousePosCallback;
	std::function<void(double x, double y)> scrollCallback;
	std::function<void(int key, int action)> keyCallback;
	std::function<void(int unicodeCodePoint)> charCallback;

protected:
	GLFWwindow *m_pWindow;
};

struct DukeGLFWApplication: public noncopyable {
	DukeGLFWApplication();
	~DukeGLFWApplication();

	GLFWwindow *createRawWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);

	template<typename WINDOW>
	WINDOW* createWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {
		return new WINDOW(createRawWindow(width, height, title, monitor, share));
	}
};

} /* namespace duke */
#endif /* GLAPP_H_ */
