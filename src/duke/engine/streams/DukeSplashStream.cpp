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
	const size_t zoom = 3;
	const size_t glyphWidth = zoom * 8;
	const auto time = context.liveTime.asMilliseconds();
	const auto bound = renderer.begin(context.viewport);
	const char greetingsString[] = "Duke R0XX!!!";
	for (size_t i = 0; i < sizeof(greetingsString); ++i) {
		Animation<dvec2> pos = Animation<dvec2>(500, dvec2(-500, 100), dvec2(100, 100)).startIn(500);
		Animation<double> alpha = Animation<double>(500, 0, 1).startIn(500);
		ivec2 offset(glyphWidth * i, 0);
		int64_t letterTime = time - 50 * i;
		drawLetter(renderer, //
				greetingsString[i], //
				zoom, //
				alpha.getAnimatedValue(letterTime, EasingCurveTimeInterpolator(EasingCurve::InQuad)), //
				offset + ivec2(pos.getAnimatedValue(letterTime, EasingCurveTimeInterpolator(EasingCurve::OutBack, 0, 0, 1.5))));
	}
	const char insertCoinString[] = "INSERT COIN";
	for (size_t i = 0; i < sizeof(greetingsString); ++i) {
		Animation<double> alpha = Animation<double>(700, 0, 1).startIn(2000);
		alpha.repeatCount = RepeatCount::INFINITE;
		alpha.repeatMode = RepeatMode::REVERSE;
		drawLetter(renderer, //
				insertCoinString[i], //
				zoom, //
				alpha.getAnimatedValue(time, EasingCurveTimeInterpolator(EasingCurve::InOutExpo)), //
				ivec2(context.viewport.dimension.x - 100 - (sizeof(greetingsString) - i) * glyphWidth, 100));
	}
}

} /* namespace duke */
