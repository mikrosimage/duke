/*
 * DukeApplication.h
 *
 *  Created on: Mar 3, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef DUKEAPPLICATION_H_
#define DUKEAPPLICATION_H_

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
#endif /* DUKEAPPLICATION_H_ */
