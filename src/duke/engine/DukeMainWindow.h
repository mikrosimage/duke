/*
 * DukeMainWindow.h
 *
 *  Created on: Mar 3, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef DUKEMAINWINDOW_H_
#define DUKEMAINWINDOW_H_

#include <duke/cmdline/CmdLineParameters.h>
#include <duke/engine/Player.h>
#include <duke/engine/Context.h>
#include <duke/engine/rendering/ShaderPool.h>
#include <duke/engine/rendering/MeshPool.h>
#include <duke/engine/rendering/GeometryRenderer.h>
#include <duke/engine/rendering/GlyphRenderer.h>
#include <duke/gl/GlFwApp.h>

namespace duke {

class DukeMainWindow: public DukeGLFWWindow {
public:
	DukeMainWindow(GLFWwindow *pWindow, const CmdLineParameters &parameters);

	void load(const Timeline& timeline, const FrameDuration& frameDuration, const FitMode fitMode, int speed);
	void run();
private:
	void onKey(int key, int action);
	void onChar(int unicodeCodePoint);
	void onWindowResize(int width, int height);
	void onMouseMove(int x, int y);
	void onMouseDrag(int dx, int dy);
	void onMouseClick(int buttonId, int buttonState);
	void onScroll(double x, double y);

	bool togglePlayStop();

	glm::ivec2 m_MousePos;
	glm::ivec2 m_WindowDim;
	glm::ivec2 m_WindowPos;
	std::vector<int> m_CharStrokes;
	std::vector<int> m_KeyStrokes;
	bool m_MouseLeftDown = false;

	Player m_Player;
	GeometryRenderer m_GeometryRenderer;
	GlyphRenderer m_GlyphRenderer;
	Context m_Context;

	glm::ivec2 m_Pan;
	double m_Zoom;
};

} /* namespace duke */
#endif /* DUKEMAINWINDOW_H_ */
