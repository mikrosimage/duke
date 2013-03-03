/*
 * CacheOverlay.cpp
 *
 *  Created on: Feb 21, 2013
 *      Author: Nicolas Rondaud
 */

#include "CacheOverlay.h"
#include <duke/engine/Context.h>
#include <duke/engine/rendering/GlyphRenderer.h>
#include <duke/engine/rendering/GeometryRenderer.h>
#include <sstream>

namespace duke {

CacheOverlay::CacheOverlay(const GlyphRenderer & glyphRenderer, const std::map<const IMediaStream*, std::vector<Range> > & state, const Timeline& timeline) :
		m_GlyphRenderer(glyphRenderer), m_CacheState(state), m_Timeline(timeline){
}

void CacheOverlay::render(const Context& context) const {
	if (!context.pCurrentImage)
		return;

	const size_t frameCount = m_Timeline.getRange().last - m_Timeline.getRange().first + 1;
	const float frameLength = context.viewport.dimension.x / (float) frameCount;
	const glm::ivec2 halfViewportDim = context.viewport.dimension / 2;
	const size_t height = 10;
	const int xOffset = -halfViewportDim.x;
	const int yOffset = -halfViewportDim.y + height / 2;

	// draw background
	const auto& geometryRenderer = m_GlyphRenderer.getGeometryRenderer();
	geometryRenderer.drawRect(context.viewport.dimension, glm::ivec2(context.viewport.dimension.x, height), //size
	glm::ivec2(0, yOffset), //pan
	glm::vec4(1, 1, 1, 0.2)); //color

	// draw cache state
	for (const Track &track : m_Timeline) {
		for (const auto & clip : track) {
			if (clip.second.pStream) {
				const auto it = m_CacheState.find(clip.second.pStream.get());
				if (it != m_CacheState.end())
					for (const auto range : it->second) {
						size_t rangeLength = frameLength * (range.last - range.first + 1);
						geometryRenderer.drawRect(context.viewport.dimension, glm::ivec2(rangeLength, height), //size
						glm::ivec2(frameLength * (clip.first + range.first) + rangeLength / 2.f + xOffset, yOffset), //pan
						glm::vec4(1, 1, 1, 0.4)); //color
					}
			}
		}
	}

	// draw cursor
	geometryRenderer.drawRect(context.viewport.dimension, glm::ivec2((frameLength < 1.0) ? 1.0 : frameLength, height), //size
	glm::ivec2(frameLength * context.currentFrame.round() + frameLength / 2.f + xOffset, yOffset), //pan
	glm::vec4(1, 1, 1, 1)); //color

	//  draw current frame
	std::ostringstream oss;
	oss << context.currentFrame.round();
	drawText(m_GlyphRenderer, context.viewport, oss.str().c_str(), 5, height + 10, 1.f, 1.f);
}

} /* namespace duke */
