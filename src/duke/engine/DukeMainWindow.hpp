#pragma once

#include "duke/cmdline/CmdLineParameters.hpp"
#include "duke/commands/Commands.hpp"
#include "duke/engine/parameters/Parameters.hpp"
#include "duke/engine/Player.hpp"
#include "duke/engine/Context.hpp"
#include "duke/engine/rendering/ShaderPool.hpp"
#include "duke/engine/rendering/MeshPool.hpp"
#include "duke/engine/rendering/GeometryRenderer.hpp"
#include "duke/engine/rendering/GlyphRenderer.hpp"
#include "duke/gl/GlFwApp.hpp"

namespace duke {

class DukeMainWindow : public DukeGLFWWindow {
 public:
  DukeMainWindow(GLFWwindow *pWindow, const CmdLineParameters &parameters);

  void load(const Timeline &timeline, const FrameDuration &frameDuration, const FitMode fitMode, int speed);
  void run();

 private:
  void onKey(int key, int action);
  void onChar(unsigned int unicodeCodePoint);
  void onWindowResize(int width, int height);
  void onMouseMove(int x, int y);
  void onMouseDrag(int dx, int dy);
  void onMouseClick(int buttonId, int buttonState);
  void onScroll(double x, double y);

  bool togglePlayStop();

  glm::ivec2 m_MousePos;
  glm::ivec2 m_WindowDim;
  glm::ivec2 m_WindowPos;
  std::vector<unsigned int> m_CharStrokes;
  std::vector<int> m_KeyStrokes;
  bool m_MouseLeftDown = false;

  const CmdLineParameters &m_CmdLine;
  Player m_Player;
  GeometryRenderer m_GeometryRenderer;
  GlyphRenderer m_GlyphRenderer;
  Context m_Context;

  cmd::Commands m_Commands;
  Parameters m_Parameters;
};

} /* namespace duke */
