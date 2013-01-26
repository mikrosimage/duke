/*
 * GlApp.cpp
 *
 *  Created on: Nov 25, 2012
 *      Author: Guillaume Chatelet
 */

#include "GlFwApp.h"

#include <duke/gl/GL.h>

#include <stdexcept>
#include <cassert>
#include <map>

namespace duke {

static void onGlfwError(int, const char* pMessage) {
	printf("An error occurred within Glfw : %s\n", pMessage);
}

static std::map<GLFWwindow*, DukeGLFWWindow*> g_GlfwToDukeWindowMap;

static DukeGLFWWindow* safeGetDukeWindow(GLFWwindow *pWindow) {
	auto pFound = g_GlfwToDukeWindowMap.find(pWindow);
	if (pFound == g_GlfwToDukeWindowMap.end())
		throw std::runtime_error("Trying to reach an inexistent window");
	return pFound->second;
}
static int onWindowCloseFun(GLFWwindow *pWindow) {
	auto pDukeWindow = safeGetDukeWindow(pWindow);
	if (pDukeWindow->windowCloseCallback)
		return pDukeWindow->windowCloseCallback();
	return GL_TRUE;
}
static void onWindowFocusFun(GLFWwindow *pWindow, int focus) {
	auto pDukeWindow = safeGetDukeWindow(pWindow);
	if (pDukeWindow->windowFocusCallback)
		pDukeWindow->windowFocusCallback(focus);
}
static void onWindowResizeFun(GLFWwindow *pWindow, int width, int height) {
	auto pDukeWindow = safeGetDukeWindow(pWindow);
	if (pDukeWindow->windowResizeCallback)
		pDukeWindow->windowResizeCallback(width, height);
}
static void onWindowRefreshFun(GLFWwindow *pWindow) {
	auto pDukeWindow = safeGetDukeWindow(pWindow);
	if (pDukeWindow->windowRefreshFunCallback)
		pDukeWindow->windowRefreshFunCallback();
}
static void onMouseButtonFun(GLFWwindow *pWindow, int buttonId, int buttonState) {
	auto pDukeWindow = safeGetDukeWindow(pWindow);
	if (pDukeWindow->mouseButtonCallback)
		pDukeWindow->mouseButtonCallback(buttonId, buttonState);
}
static void onMousePosFun(GLFWwindow *pWindow, int x, int y) {
	auto pDukeWindow = safeGetDukeWindow(pWindow);
	if (pDukeWindow->mousePosCallback)
		pDukeWindow->mousePosCallback(x, y);
}
static void onMouseWheelFun(GLFWwindow *pWindow, double x, double y) {
	auto pDukeWindow = safeGetDukeWindow(pWindow);
	if (pDukeWindow->scrollCallback)
		pDukeWindow->scrollCallback(x, y);
}
static void onKeyFun(GLFWwindow *pWindow, int keyId, int keyState) {
	auto pDukeWindow = safeGetDukeWindow(pWindow);
	if (pDukeWindow->keyCallback)
		pDukeWindow->keyCallback(keyId, keyState);
}
static void onCharFun(GLFWwindow *pWindow, int unicodeCodePoint) {
	auto pDukeWindow = safeGetDukeWindow(pWindow);
	if (pDukeWindow->charCallback)
		pDukeWindow->charCallback(unicodeCodePoint);
}

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
	g_GlfwToDukeWindowMap[m_pWindow] = this;
	glfwMakeContextCurrent(m_pWindow);

}

DukeGLFWWindow::~DukeGLFWWindow() {
	glfwDestroyWindow(m_pWindow); // all callbacks disabled
	g_GlfwToDukeWindowMap.erase(m_pWindow);
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
