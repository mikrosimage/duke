#include "StatisticsOverlay.hpp"
#include <duke/engine/Context.hpp>
#include <duke/engine/rendering/GlyphRenderer.hpp>
#include <duke/engine/rendering/GeometryRenderer.hpp>
#include <sstream>

namespace duke {

StatisticsOverlay::StatisticsOverlay(const GlyphRenderer & glyphRenderer, const Timeline& timeline) :
		vBlankMetronom(100), frameMetronom(10), m_GlyphRenderer(glyphRenderer), m_Timeline(timeline) {
}

void StatisticsOverlay::render(const Context& context) const {
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
	glm::vec4(1, 1, 1, 0.2),//color
	context.isPlaying);

	// draw cache state
	for (const Track &track : m_Timeline) {
		for (const auto & clip : track) {
			if (clip.second.pStream) {
				const auto it = cacheState.find(clip.second.pStream.get());
				if (it != cacheState.end())
					for (const auto range : it->second) {
						size_t rangeLength = frameLength * (range.last - range.first + 1);
						geometryRenderer.drawRect(context.viewport.dimension, glm::ivec2(rangeLength, height), //size
						glm::ivec2(frameLength * (clip.first + range.first) + rangeLength / 2.f + xOffset, yOffset), //pan
						glm::vec4(1, 1, 1, 0.4), //color
						context.isPlaying); 
					}
			}
		}
	}

	// draw cursor
	geometryRenderer.drawRect(context.viewport.dimension, glm::ivec2((frameLength < 1.0) ? 1.0 : frameLength, height), //size
	glm::ivec2(frameLength * context.currentFrame.round() + frameLength / 2.f + xOffset, yOffset), //pan
	glm::vec4(1, 1, 1, 1),//color
	context.isPlaying); 

	//  draw infos
	std::ostringstream oss;

	oss << context.currentFrame.round() << '\n';
	oss << std::fixed;
	oss.width(5);
	oss.precision(2);
	oss << frameMetronom.getFPS() << "  FPS" << '\n';
	oss << "zoom " << context.zoom << "x";
#ifndef NDEBUG // adding vblank in case in debug mode
	oss << '\n' << vBlankMetronom.getFPS() << " VBPS";
#endif
	drawText(m_GlyphRenderer, context.viewport, oss.str().c_str(), 5, height + 10, 1.f, 1.f, context.isPlaying);
}

} /* namespace duke */
