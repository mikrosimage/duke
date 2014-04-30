#include "DukeSplashStream.hpp"
#include <duke/engine/Context.hpp>
#include <duke/animation/Animation.hpp>
#include <glm/glm.hpp>

using namespace glm;

namespace duke {

DukeSplashStream::DukeSplashStream() {
  m_LeftAlpha.duration = 500;
  m_LeftAlpha.startTime = 500;
  m_LeftAlpha.type = EasingCurve::InQuad;
  m_LeftPos.duration = 500;
  m_LeftPos.startTime = 500;
  m_LeftPos.type = EasingCurve::OutBack;
  m_LeftPos.overshoot = 1.5;
  m_RightAlpha.duration = 1000;
  m_RightAlpha.startTime = 2000;
  m_RightAlpha.type = EasingCurve::InOutExpo;
  m_RightAlpha.repeatCount = RepeatCount::INFINITE;
  m_RightAlpha.repeatMode = RepeatMode::REVERSE;
}

DukeSplashStream::~DukeSplashStream() {}

namespace {

void drawLetter(const GlyphRenderer& renderer, char c, float zoom, float alpha, ivec2 position) {
  renderer.setAlpha(alpha);
  renderer.setZoom(zoom);
  renderer.draw(position.x, position.y, c);
}

}  // namespace

void DukeSplashStream::render(const Context& context) const {
  const size_t zoom = 2;
  const size_t glyphWidth = zoom * 8;
  const auto time = context.liveTime.asMilliseconds();
  const auto& renderer = *context.pGlyphRenderer;
  const auto bound = renderer.begin(context.viewport);
  {
    const char greetingsString[] = "Duke R0XX!!!";
    for (size_t i = 0; i < sizeof(greetingsString); ++i) {
      const ivec2 offset(glyphWidth * i, 0);
      const int64_t letterTime = time - 50 * i;
      const float alpha = interpolateValue<float>(m_LeftAlpha, 0, 1, letterTime);
      const dvec2 position = interpolateValue<dvec2>(m_LeftPos, dvec2(-500, 100), dvec2(100, 100), letterTime);
      drawLetter(renderer,            //
                 greetingsString[i],  //
                 zoom,                //
                 alpha,               //
                 offset + ivec2(position));
    }
  }
  {
    const char insertCoinString[] = "INSERT FRAMES";
    for (size_t i = 0; i < sizeof(insertCoinString); ++i) {
      drawLetter(renderer,                                           //
                 insertCoinString[i],                                //
                 zoom,                                               //
                 interpolateValue<float>(m_RightAlpha, 0, 1, time),  //
                 ivec2(context.viewport.dimension.x - 100 - (sizeof(insertCoinString) - i) * glyphWidth, 100));
    }
  }
}

} /* namespace duke */
