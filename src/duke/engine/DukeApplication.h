/*
 * DukeApplication.h
 *
 *  Created on: Mar 3, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <duke/engine/DukeMainWindow.h>
#include <duke/gl/GlFwApp.h>

namespace duke {

struct CmdLineParameters;

class DukeApplication: private DukeGLFWApplication {
public:
	DukeApplication(const CmdLineParameters &parameters);
	void run();
private:
	DukeMainWindow m_MainWindow;
};

} /* namespace duke */
