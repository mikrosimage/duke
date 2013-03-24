/*
 * CmdLineParameters.h
 *
 *  Created on: Dec 2, 2012
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <vector>
#include <string>
#include <stdexcept>

#include <duke/time/FrameUtils.h>

namespace duke {

struct commandline_error: std::runtime_error {
	commandline_error(const std::string& msg) :
			std::runtime_error(msg) {
	}
};

enum class ApplicationMode {
	DUKE, BENCHMARK, HELP, VERSION
};

struct CmdLineParameters {
	CmdLineParameters(int argc, char**argv);
	void printHelpMessage() const;
	unsigned swapBufferInterval = 1;
	bool fullscreen = false;
	bool unlimitedFPS = false;
	unsigned workerThreadDefault = getDefaultConsurrency();
	size_t imageCacheSizeDefault = getDefaultCacheSize();
	ApplicationMode mode = ApplicationMode::DUKE;
	FrameDuration defaultFrameRate = FrameDuration::PAL;
	std::vector<std::string> additionnalOptions;

	static unsigned getDefaultConsurrency();
	static size_t getDefaultCacheSize();
};

}  // namespace duke
