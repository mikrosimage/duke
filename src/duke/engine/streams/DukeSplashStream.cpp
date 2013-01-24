/*
 * DukeSplashStream.cpp
 *
 *  Created on: Jan 24, 2013
 *      Author: Guillaume Chatelet
 */

#include "DukeSplashStream.h"
#include <duke/engine/Context.h>
#include <glm/glm.hpp>

namespace duke {

DukeSplashStream::~DukeSplashStream() {
}

static void drawLetter(const GlyphRenderer& renderer, char c, double time, double xCenter, double yCenter, double offset) {
    const double x = xCenter + offset * cos(time);
    const double y = yCenter + offset * sin(time * 3.14159265359 / 2);
    const double zoom = 1 + (cos(time) + 1) * 2;
    renderer.setPosition(x, y);
    renderer.setAlpha(1 - (1 / zoom));
    renderer.setZoom(zoom);
    renderer.draw(c);
}

void DukeSplashStream::doRender(const Context& context) const {
    using namespace glm;
    const double time = context.liveTime.asDouble();
    const ivec2 center = context.viewport.dimension / 2;
    const int offset = center.x / 3;
    const auto bound = renderer.begin(context.viewport);
    drawLetter(renderer, 'D', time, center.x, center.y, offset);
    drawLetter(renderer, 'u', time - .5, center.x, center.y, offset);
    drawLetter(renderer, 'k', time - 1, center.x, center.y, offset);
    drawLetter(renderer, 'e', time - 1.5, center.x, center.y, offset);
}

} /* namespace duke */
