/*
 * Duke.h
 *
 *  Created on: Jan 6, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef DUKE_H_
#define DUKE_H_

#include <duke/NonCopyable.h>
#include <duke/engine/DukeWindow.h>
#include <duke/engine/Player.h>
#include <duke/engine/Context.h>

namespace duke {

struct CmdLineParameters;

class Duke: public noncopyable {
public:
	Duke(CmdLineParameters parameters);
	void run();
private:
	void cue(int offset);
	void togglePlayStop();

	bool keyPressed(int) const;
	bool hasWindowParam(int) const;

	DukeGLFWApplication m_Application;
	std::unique_ptr<DukeWindow> m_pWindow;
	Player m_Player;
	Context m_Context;
};

} /* namespace duke */
#endif /* DUKE_H_ */
