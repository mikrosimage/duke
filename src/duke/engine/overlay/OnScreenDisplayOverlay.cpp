#include "OnScreenDisplayOverlay.hpp"
#include <duke/engine/rendering/GlyphRenderer.hpp>
#include <duke/engine/Context.hpp>
#include <duke/animation/Animation.hpp>
#include <algorithm>

namespace duke {

OnScreenDisplayOverlay::OnScreenDisplayOverlay(const GlyphRenderer& glyphRenderer) : m_GlyphRenderer(glyphRenderer) {
  m_Alpha.duration = 800;
  m_Alpha.type = EasingCurve::InExpo;
}

void OnScreenDisplayOverlay::setString(const Time& time, const std::string& msg) {
  m_ShowTime = time;
  m_Message = msg;
}

void OnScreenDisplayOverlay::render(const Context& context) const {
  Time time = context.liveTime;
  time -= m_ShowTime;
  const auto ms = time.asMilliseconds();
  const double alpha = interpolateValue<double>(m_Alpha, 1, 0, ms);
  if (alpha > 0)
    drawText(m_GlyphRenderer,    //
             context.viewport,   //
             m_Message.c_str(),  //
             100, 100,           //
             alpha,              //
             3);
}

} /* namespace duke */
