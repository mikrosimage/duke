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
#include <algorithm>
#include <thread>

#include <duke/time/FrameUtils.h>

namespace duke {

struct commandline_error: std::runtime_error {
	commandline_error(const std::string& msg) :
			std::runtime_error(msg) {
	}
};

enum class ApplicationMode {
	DUKE, BENCHMARK, HELP
};

struct CmdLineParameters {
	CmdLineParameters(int argc, char**argv);
	const char* getHelpMessage() const;
	unsigned swapBufferInterval = 1;
	bool fullscreen = false;
	unsigned workerThreadDefault = std::max(1u, std::min(4u, std::thread::hardware_concurrency() - 2));
	size_t imageCacheSizeDefault = 500 * 1024 * 1024; // 500MiB
	ApplicationMode mode = ApplicationMode::DUKE;
	FrameDuration defaultFrameRate = FrameDuration::PAL;
	std::vector<std::string> additionnalOptions;
};

}  // namespace duke

#endif /* CMDLINEPARAMETERS_H_ */
