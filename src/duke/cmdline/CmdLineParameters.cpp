/*
 * CmdLineParameters.cpp
 *
 *  Created on: Dec 2, 2012
 *      Author: Guillaume Chatelet
 */

#include "CmdLineParameters.h"

#include <stdexcept>
#include <sstream>
#include <cstring>
#include <string>

using namespace std;

template<typename T>
static void getArgs(const int argc, char**argv, int i, T& value) {
	if (i >= argc)
		throw logic_error("missing command line value");
	istringstream iss(argv[i]);
	iss >> value;
	if (iss.fail())
		throw logic_error("bad command line argument type");
}

static bool matches(const string option, const string shortOption, const string longOption) {
	return option == shortOption || option == longOption;
}

namespace duke {

CmdLineParameters::CmdLineParameters(int argc, char**argv) {
	for (int i = 1; i < argc; ++i) {
		const char* pOption = argv[i];
		if (matches(pOption, "", "--swapinterval"))
			getArgs(argc, argv, ++i, swapBufferInterval);
		else if (matches(pOption, "-f", "--fullscreen"))
			fullscreen = true;
		else if (matches(pOption, "-t", "--threads"))
			getArgs(argc, argv, ++i, workerThreadDefault);
		else if (matches(pOption, "-s", "--cache-size")) {
			getArgs(argc, argv, ++i, imageCacheSizeDefault);
			imageCacheSizeDefault *= 1024 * 1024;
		} else if (matches(pOption, "", "--benchmark"))
			mode = ApplicationMode::BENCHMARK;
		else if (matches(pOption, "-h", "--help"))
			mode = ApplicationMode::HELP;
		else if (*pOption != '-')
			additionnalOptions.push_back(pOption);
		else
			throw logic_error(string("unknown command line argument '") + pOption + "'");
	}
}

const char* CmdLineParameters::getHelpMessage() const {
	return R"(Usage: duke [OPTION]... [FILE/FOLDER]...
  -h, --help                 displays this message
      --swapinterval         specifies n as the mandatory count of wait for
                             vblank before displaying a frame, default is 1.
  -t, --threads              specify the number of decoding threads.
  -s, --cache-size           size of the in-memory cache system in MB.
  -f, --fullscreen           switch to fullscreen mode.
      --benchmark            tests current machine's performance.
)";
}

}  // namespace duke
