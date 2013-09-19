#include "CmdLineParameters.hpp"

#include <duke/memory/AvailableMemory.hpp>

#include <stdexcept>
#include <sstream>
#include <cstring>
#include <string>
#include <algorithm>
#include <thread>

using namespace std;

namespace duke {

namespace {

void checkArgCount(const int argc, int i) {
    if (i >= argc) throw logic_error("missing command line value");
}

template<typename T>
void getArgs(const int argc, const char* const * argv, int i, T& value) {
    checkArgCount(argc, i);
    istringstream iss(argv[i]);
    iss >> value;
    if (iss.fail()) throw logic_error("bad command line argument type");
}
template<>
void getArgs<string>(const int argc, const char* const * argv, int i, string& value) {
    checkArgCount(argc, i);
    value = argv[i];
}

bool matches(const string option, const string longOption, const string shortOption = "") {
    return option == shortOption || option == longOption;
}

}  // namespace

unsigned CmdLineParameters::getDefaultConcurrency() {
    return max(1u, min(4u, thread::hardware_concurrency() - 2));
}

size_t CmdLineParameters::getDefaultCacheSize() {
    return 500 * 1024 * 1024; // 500MiB
}

CmdLineParameters::CmdLineParameters(int argc, const char* const * argv) {
    for (int i = 1; i < argc; ++i) {
        const char* pOption = argv[i];
        if (matches(pOption, "--swapinterval"))
            getArgs(argc, argv, ++i, swapBufferInterval);
        else if (matches(pOption, "--fullscreen", "-f"))
            fullscreen = true;
        else if (matches(pOption, "--unlimited"))
            unlimitedFPS = true;
        else if (matches(pOption, "--threads", "-t"))
            getArgs(argc, argv, ++i, workerThreadDefault);
        else if (matches(pOption, "--max-cache-size")) {
            imageCacheSizeDefault = getTotalSystemMemory() * 80 / 100;
        } else if (matches(pOption, "--cache-size", "-s")) {
            getArgs(argc, argv, ++i, imageCacheSizeDefault);
            imageCacheSizeDefault *= 1024 * 1024;
        } else if (matches(pOption, "--framerate")) {
            string arg;
            getArgs(argc, argv, ++i, arg);
            if (arg == "noskip") {
                unlimitedFPS = true;
            } else {
                istringstream stream(arg);
                stream >> defaultFrameRate;
                if (stream.fail()) throw logic_error("invalid framerate");
                defaultFrameRate.assign(defaultFrameRate.denominator(), defaultFrameRate.numerator());
            }
        } else if (matches(pOption, "--benchmark"))
            mode = ApplicationMode::BENCHMARK;
        else if (matches(pOption, "--help", "-h"))
            mode = ApplicationMode::HELP;
        else if (matches(pOption, "--version", "-v"))
            mode = ApplicationMode::VERSION;
        else if (matches(pOption, "--list", "-l"))
            mode = ApplicationMode::LIST_SUPPORTED_FORMAT;
        else if (matches(pOption, "--inputspace")) {
			std::string colorSpaceString;
			getArgs(argc, argv, ++i, colorSpaceString);
			inputColorSpace = resolveFromName(colorSpaceString.c_str());
		}
        else if (matches(pOption, "--outputspace")) {
			std::string colorSpaceString;
			getArgs(argc, argv, ++i, colorSpaceString);
			outputColorSpace = resolveFromName(colorSpaceString.c_str());
		}
        else if (*pOption != '-')
            additionnalOptions.push_back(pOption);
        else
            throw logic_error(string("unknown command line argument '") + pOption + "'");
    }
}

void CmdLineParameters::printHelpMessage() const {
    printf(R"(Usage: duke [OPTION]... [FILE/FOLDER]...
  -h, --help                 display this help and exit
  -v, --version              output version information and exit
      --benchmark            tests current machine's performance.
      --swapinterval SIZE    specifies SIZE mandatory count of wait for
                             vblank before displaying a frame, default is 1.

      --framerate noskip     play every frame - no particular framerate.
      --framerate FPS        sets framerate to FPS, can be 
                               integer (25),
                               floating point (29.97)
                               rational number (30000/1001)

      --inputspace           force the file colorspace
                             [Linear, sRGB, Rec709, AlexaV3LogC, KodakLog]

      --outputspace          force the display colorspace
                             [Linear, sRGB, Rec709]

  -f, --fullscreen           switch to fullscreen mode.
  -l, --list-formats         output supported formats and exit
  -s, --cache-size SIZE      size of the in-memory cache system in MiB,
                             default is %lu.
      --max-cache-size       size of the in-memory cache system set to 80%%
                             of machine memory.
  -t, --threads SIZE         specify the number of decoding threads,
                             defaults to %u for this machine.
)",
           getDefaultCacheSize() / (1024 * 1024), getDefaultConcurrency());
}

}  // namespace duke
