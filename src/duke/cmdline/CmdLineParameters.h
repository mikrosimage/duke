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

enum class ApplicationMode {
    DUKE, BENCHMARK, HELP
};

struct CmdLineParameters {
	CmdLineParameters(int argc, char**argv);
	const char* getHelpMessage() const;
	unsigned swapBufferInterval = 1;
	bool fullscreen = false;
	ApplicationMode mode = ApplicationMode::DUKE;
	std::vector<std::string> additionnalOptions;
};

}  // namespace duke

#endif /* CMDLINEPARAMETERS_H_ */
