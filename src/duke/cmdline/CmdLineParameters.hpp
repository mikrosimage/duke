#pragma once

#include <vector>
#include <string>
#include <stdexcept>

#include <duke/time/FrameUtils.hpp>
#include <duke/engine/ColorSpace.hpp>

namespace duke {

struct commandline_error : std::runtime_error {
  commandline_error(const std::string& msg) : std::runtime_error(msg) {}
};

enum class ApplicationMode {
  DUKE,
  BENCHMARK,
  HELP,
  VERSION,
  LIST_SUPPORTED_FORMAT
};

struct CmdLineParameters {
  CmdLineParameters(int argc, const char* const* argv);
  void printHelpMessage() const;
  unsigned swapBufferInterval = 1;
  bool fullscreen = false;
  bool unlimitedFPS = false;
  unsigned workerThreadDefault = getDefaultConcurrency();
  size_t imageCacheSizeDefault = getDefaultCacheSize();
  ApplicationMode mode = ApplicationMode::DUKE;
  FrameDuration defaultFrameRate = FrameDuration::PAL;
  std::vector<std::string> additionnalOptions;
  ColorSpace inputColorSpace = ColorSpace::Auto;
  ColorSpace outputColorSpace = ColorSpace::Auto;

  static unsigned getDefaultConcurrency();
  static size_t getDefaultCacheSize();
};

}  // namespace duke
