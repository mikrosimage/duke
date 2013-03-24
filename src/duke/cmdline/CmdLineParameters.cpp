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
#include <algorithm>
#include <thread>

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

unsigned CmdLineParameters::getDefaultConsurrency() {
    return max(1u, min(4u, thread::hardware_concurrency() - 2));
}

size_t CmdLineParameters::getDefaultCacheSize() {
    return 500 * 1024 * 1024; // 500MiB
}

CmdLineParameters::CmdLineParameters(int argc, char**argv) {
    for (int i = 1; i < argc; ++i) {
        const char* pOption = argv[i];
        if (matches(pOption, "", "--swapinterval"))
            getArgs(argc, argv, ++i, swapBufferInterval);
        else if (matches(pOption, "-f", "--fullscreen"))
            fullscreen = true;
        else if (matches(pOption, "", "--unlimited"))
            unlimitedFPS = true;
        else if (matches(pOption, "-t", "--threads"))
            getArgs(argc, argv, ++i, workerThreadDefault);
        else if (matches(pOption, "", "--film"))
            defaultFrameRate = FrameDuration::FILM;
        else if (matches(pOption, "", "--ntsc"))
            defaultFrameRate = FrameDuration::NTSC;
        else if (matches(pOption, "", "--pal"))
            defaultFrameRate = FrameDuration::PAL;
        else if (matches(pOption, "-s", "--cache-size")) {
            getArgs(argc, argv, ++i, imageCacheSizeDefault);
            imageCacheSizeDefault *= 1024 * 1024;
        } else if (matches(pOption, "", "--benchmark"))
            mode = ApplicationMode::BENCHMARK;
        else if (matches(pOption, "-h", "--help"))
            mode = ApplicationMode::HELP;
        else if (matches(pOption, "-v", "--version"))
            mode = ApplicationMode::VERSION;
        else if (*pOption != '-')
            additionnalOptions.push_back(pOption);
        else
            throw logic_error(string("unknown command line argument '") + pOption + "'");
    }
}

void CmdLineParameters::printHelpMessage() const {
    printf(R"(Usage: duke [OPTION]... [FILE/FOLDER]...
  -h, --help                 displays this message
  -v, --version              displays revision version

      --pal                  sets framerate to 25 fps (default)
      --film                 sets framerate to 24 fps
      --ntsc                 sets framerate to 29.97 fps
      --unlimited            unlimited framerate, benchmarking purpose

      --swapinterval         specifies n as the mandatory count of wait for
                             vblank before displaying a frame, default is 1.
  -t, --threads              specify the number of decoding threads,
                             defaults to %u for this machine.
  -s, --cache-size           size of the in-memory cache system in MiB,
                             default is %lu.
  -f, --fullscreen           switch to fullscreen mode.
      --benchmark            tests current machine's performance.
)", getDefaultConsurrency(), getDefaultCacheSize() / (1024 * 1024));
}

}  // namespace duke
