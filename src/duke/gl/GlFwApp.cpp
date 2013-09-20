#include "GlFwApp.hpp"

#include <duke/gl/GL.hpp>

#include <stdexcept>
#include <cassert>
#include <map>

namespace duke {

namespace  {

void onGlfwError(int, const char* pMessage) {
#ifndef NDEBUG
	printf("An error occurred within Glfw : %s\n", pMessage);
#endif
}
std::map<GLFWwindow*, DukeGLFWWindow*>& getGlfwToDukeWindowMap() {
    static std::map<GLFWwindow*, DukeGLFWWindow*> glfwToDukeWindowMap;
    return glfwToDukeWindowMap;
}
DukeGLFWWindow* safeGetDukeWindow(GLFWwindow *pWindow) {
    const auto& glfwToDukeWindowMap = getGlfwToDukeWindowMap();
    auto pFound = glfwToDukeWindowMap.find(pWindow);
    if (pFound == glfwToDukeWindowMap.end())
        throw std::runtime_error("Trying to reach an inexistent window");
    return pFound->second;
}
void onWindowCloseFun(GLFWwindow *pWindow) {
    auto pDukeWindow = safeGetDukeWindow(pWindow);
    if (pDukeWindow->windowCloseCallback)
        pDukeWindow->windowCloseCallback();
}
void onWindowFocusFun(GLFWwindow *pWindow, int focus) {
    auto pDukeWindow = safeGetDukeWindow(pWindow);
    if (pDukeWindow->windowFocusCallback)
        pDukeWindow->windowFocusCallback(focus);
}
void onWindowResizeFun(GLFWwindow *pWindow, int width, int height) {
    auto pDukeWindow = safeGetDukeWindow(pWindow);
    if (pDukeWindow->windowResizeCallback)
        pDukeWindow->windowResizeCallback(width, height);
}
void onWindowRefreshFun(GLFWwindow *pWindow) {
    auto pDukeWindow = safeGetDukeWindow(pWindow);
    if (pDukeWindow->windowRefreshFunCallback)
        pDukeWindow->windowRefreshFunCallback();
}
void onMouseButtonFun(GLFWwindow *pWindow, int buttonId, int buttonState, int modifier) {
    auto pDukeWindow = safeGetDukeWindow(pWindow);
    if (pDukeWindow->mouseButtonCallback)
        pDukeWindow->mouseButtonCallback(buttonId, buttonState);
}
void onMousePosFun(GLFWwindow *pWindow, double x, double y) {
    auto pDukeWindow = safeGetDukeWindow(pWindow);
    if (pDukeWindow->mousePosCallback)
        pDukeWindow->mousePosCallback(x, y);
}
void onMouseWheelFun(GLFWwindow *pWindow, double x, double y) {
    auto pDukeWindow = safeGetDukeWindow(pWindow);
    if (pDukeWindow->scrollCallback)
        pDukeWindow->scrollCallback(x, y);
}
void onKeyFun(GLFWwindow *pWindow, int keyId, int scancode, int keyState, int modifier) {
    auto pDukeWindow = safeGetDukeWindow(pWindow);
    if (pDukeWindow->keyCallback)
        pDukeWindow->keyCallback(keyId, keyState);
}
void onCharFun(GLFWwindow *pWindow, unsigned int unicodeCodePoint) {
    auto pDukeWindow = safeGetDukeWindow(pWindow);
    if (pDukeWindow->charCallback)
        pDukeWindow->charCallback(unicodeCodePoint);
}

}  // namespace




DukeGLFWApplication::DukeGLFWApplication() {
	if (!glfwInit())
		throw std::runtime_error("Unable to initialize OpenGL");
	glfwSetErrorCallback(&onGlfwError);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

DukeGLFWApplication::~DukeGLFWApplication() {
	glfwTerminate();
}

GLFWwindow *DukeGLFWApplication::createRawWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {
	GLFWwindow *pWindow = glfwCreateWindow(width, height, title, monitor, share);
	if (!pWindow)
		throw std::runtime_error("Unable to open window");
	return pWindow;
}

DukeGLFWWindow::DukeGLFWWindow(GLFWwindow *pWindow) :
		m_pWindow(pWindow) {
	if (!m_pWindow)
		throw std::runtime_error("Illegal creation of nullptr Window");
	getGlfwToDukeWindowMap()[m_pWindow] = this;
	glfwMakeContextCurrent(m_pWindow);

}

DukeGLFWWindow::~DukeGLFWWindow() {
	glfwDestroyWindow(m_pWindow); // all callbacks disabled
	getGlfwToDukeWindowMap().erase(m_pWindow);
}

void DukeGLFWWindow::registerCallbacks() {
	glfwSetWindowFocusCallback(m_pWindow, &onWindowFocusFun);
	glfwSetWindowCloseCallback(m_pWindow, &onWindowCloseFun);
	glfwSetWindowSizeCallback(m_pWindow, &onWindowResizeFun);
	glfwSetWindowRefreshCallback(m_pWindow, &onWindowRefreshFun);
	glfwSetCursorPosCallback(m_pWindow, &onMousePosFun);
	glfwSetMouseButtonCallback(m_pWindow, &onMouseButtonFun);
	glfwSetScrollCallback(m_pWindow, &onMouseWheelFun);
	glfwSetKeyCallback(m_pWindow, &onKeyFun);
	glfwSetCharCallback(m_pWindow, &onCharFun);
}

GLFWwindow *DukeGLFWWindow::getHandle() {
	return m_pWindow;
}

} /* namespace duke */
