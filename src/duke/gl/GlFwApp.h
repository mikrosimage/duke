/*
 * GlApp.h
 *
 *  Created on: Nov 25, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef GLAPP_H_
#define GLAPP_H_

#include <functional>

namespace duke {

struct GlFwApp {
	GlFwApp();
	~GlFwApp();

	void openWindow(int width, int height, int redbits, int greenbits, int bluebits, int alphabits, int depthbits, int stencilbits, int mode);
	void closeWindow();

	std::function<int()> windowCloseCallback;
	std::function<void(int width, int height)> windowResizeCallback;
	std::function<void()> windowRefreshFunCallback;
	std::function<void(int buttonId, int buttonState)> mouseButtonCallback;
	std::function<void(int x, int y)> mousePosCallback;
	std::function<void(int value)> mouseWheelCallback;
	std::function<void(int keyId, int keyState)> keyCallback;
	std::function<void(int unicodeCodePoint, int keyState)> charCallback;
private:
	friend int onWindowCloseFun();
	friend void onWindowResizeFun(int, int);
	friend void onWindowRefreshFun();
	friend void onMouseButtonFun(int, int);
	friend void onMousePosFun(int, int);
	friend void onMouseWheelFun(int);
	friend void onKeyFun(int, int);
	friend void onCharFun(int, int);

	GlFwApp(const GlFwApp&);
	void operator=(const GlFwApp&);
};

} /* namespace duke */
#endif /* GLAPP_H_ */
