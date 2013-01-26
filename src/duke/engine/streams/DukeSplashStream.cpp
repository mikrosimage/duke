/*
 * DukeSplashStream.cpp
 *
 *  Created on: Jan 24, 2013
 *      Author: Guillaume Chatelet
 */

#include "DukeSplashStream.h"
#include <duke/engine/Context.h>
#include <duke/animation/Animation.h>
#include <glm/glm.hpp>

using namespace glm;

namespace duke {

DukeSplashStream::~DukeSplashStream() {
}

static void drawLetter(const GlyphRenderer& renderer, char c, float zoom, float alpha, ivec2 position) {
	renderer.setPosition(position.x, position.y);
	renderer.setAlpha(alpha);
	renderer.setZoom(zoom);
	renderer.draw(c);
}

void DukeSplashStream::doRender(const Context& context) const {
	const auto time = context.liveTime.asMilliseconds();
	const auto bound = renderer.begin(context.viewport);
	const char string[] = "Duke R0XX!!!";
	for (size_t i = 0; i < sizeof(string); ++i) {
		Animation<dvec2> pos(500, dvec2(-500, 100), dvec2(100, 100));
		Animation<double> alpha(500, 0, 1);
		ivec2 offset(4 * 8 * i, 0);
		int64_t letterTime = time - 50 * i;
		drawLetter(renderer, //
				string[i], //
				4, //
				alpha.getAnimatedValue(letterTime, EasingCurveTimeInterpolator(EasingCurve::InQuad)), //
				offset + ivec2(pos.getAnimatedValue(letterTime, EasingCurveTimeInterpolator(EasingCurve::OutBack, 0, 0, 1.5))));
	}
}

} /* namespace duke */
