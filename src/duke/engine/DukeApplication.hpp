#pragma once

#include "duke/engine/DukeMainWindow.hpp"
#include "duke/gl/GlFwApp.hpp"

namespace duke {

struct CmdLineParameters;

class DukeApplication : private DukeGLFWApplication {
 public:
  DukeApplication(const CmdLineParameters &parameters);
  void run();

 private:
  DukeMainWindow m_MainWindow;
};

} /* namespace duke */
