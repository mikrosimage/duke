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

namespace duke {

GlFwApp * g_pGlFwApp = nullptr;

int GLFWCALL onWindowCloseFun() {
	assert(g_pGlFwApp);
	if (g_pGlFwApp->windowCloseCallback)
		return g_pGlFwApp->windowCloseCallback();
	return GL_TRUE;
}
void GLFWCALL onWindowResizeFun(int width, int height) {
	assert(g_pGlFwApp);
	if (g_pGlFwApp->windowResizeCallback)
		g_pGlFwApp->windowResizeCallback(width, height);
}
void GLFWCALL onWindowRefreshFun() {
	assert(g_pGlFwApp);
	if (g_pGlFwApp->windowRefreshFunCallback)
		g_pGlFwApp->windowRefreshFunCallback();
}
void GLFWCALL onMouseButtonFun(int buttonId, int buttonState) {
	assert(g_pGlFwApp);
	if (g_pGlFwApp->mouseButtonCallback)
		g_pGlFwApp->mouseButtonCallback(buttonId, buttonState);
}
void GLFWCALL onMousePosFun(int x, int y) {
	assert(g_pGlFwApp);
	if (g_pGlFwApp->mousePosCallback)
		g_pGlFwApp->mousePosCallback(x, y);
}
void GLFWCALL onMouseWheelFun(int value) {
	assert(g_pGlFwApp);
	if (g_pGlFwApp->mouseWheelCallback)
		g_pGlFwApp->mouseWheelCallback(value);
}
void GLFWCALL onKeyFun(int keyId, int keyState) {
	assert(g_pGlFwApp);
	if (g_pGlFwApp->keyCallback)
		g_pGlFwApp->keyCallback(keyId, keyState);
}
void GLFWCALL onCharFun(int unicodeCodePoint, int keyState) {
	assert(g_pGlFwApp);
	if (g_pGlFwApp->charCallback)
		g_pGlFwApp->charCallback(unicodeCodePoint, keyState);
}

GlFwApp::GlFwApp() {
	if (g_pGlFwApp)
		throw std::runtime_error("Only one GlFwApp is allowed");
	g_pGlFwApp = this;
	if (!glfwInit())
		throw std::runtime_error("Unable to initialize OpenGL");
}

GlFwApp::~GlFwApp() {
	closeWindow();
	glfwTerminate();
	g_pGlFwApp = nullptr;
}

void GlFwApp::openWindow(int width, int height, int redbits, int greenbits, int bluebits, int alphabits, int depthbits, int stencilbits, int mode) {
	if (!glfwOpenWindow(width, height, redbits, greenbits, bluebits, alphabits, depthbits, stencilbits, mode))
		throw std::runtime_error("Unable to open window");
	glfwSetWindowCloseCallback(&onWindowCloseFun);
	glfwSetWindowSizeCallback(&onWindowResizeFun);
	glfwSetWindowRefreshCallback(&onWindowRefreshFun);
	glfwSetMousePosCallback(&onMousePosFun);
	glfwSetMouseButtonCallback(&onMouseButtonFun);
	glfwSetMouseWheelCallback(&onMouseWheelFun);
	glfwSetKeyCallback(&onKeyFun);
	glfwSetCharCallback(&onCharFun);
}

void GlFwApp::closeWindow() {
	glfwSetWindowCloseCallback(nullptr);
	glfwSetWindowSizeCallback(nullptr);
	glfwSetWindowRefreshCallback(nullptr);
	glfwSetMousePosCallback(nullptr);
	glfwSetMouseButtonCallback(nullptr);
	glfwSetMouseWheelCallback(nullptr);
	glfwSetKeyCallback(nullptr);
	glfwSetCharCallback(nullptr);
}

} /* namespace duke */
