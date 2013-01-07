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


namespace duke {

struct CmdLineParameters;

class Duke: public noncopyable {
public:
	Duke(const CmdLineParameters &parameters);
	void run();
private:
	DukeWindow m_Window;
	Player m_Player;
};

} /* namespace duke */
#endif /* DUKE_H_ */
