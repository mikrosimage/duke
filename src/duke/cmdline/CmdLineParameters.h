/*
 * CmdLineParameters.h
 *
 *  Created on: Dec 2, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef CMDLINEPARAMETERS_H_
#define CMDLINEPARAMETERS_H_

#include <vector>
#include <string>
#include <stdexcept>

namespace duke {

struct commandline_error : std::runtime_error {
	commandline_error(const std::string& msg) : std::runtime_error(msg){}
};

struct CmdLineParameters {
	CmdLineParameters(int argc, char**argv);
	unsigned swapBufferInterval = 0;
	bool fullscreen = false;
	std::vector<std::string> additionnalOptions;
};

}  // namespace duke

#endif /* CMDLINEPARAMETERS_H_ */
