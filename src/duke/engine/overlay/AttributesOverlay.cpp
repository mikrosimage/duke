#include "AttributesOverlay.hpp"
#include <duke/engine/Context.hpp>
#include <duke/engine/rendering/GlyphRenderer.hpp>
#include <duke/imageio/PackedFrameDescriptionAndAttributes.hpp>
#include <algorithm>

namespace duke {

AttributesOverlay::AttributesOverlay(const GlyphRenderer &glyphRenderer) :
                m_GlyphRenderer(glyphRenderer) {
	registerPrimitiveTypes(m_AttributeDisplay);
}

void AttributesOverlay::render(const Context &context) const {
    if (!context.pCurrentImage) return;

    const Attributes& attributes = context.pCurrentImage->attributes;

    size_t maxKeyCharacters = 0;
    for (const auto& entry : attributes) {
    	maxKeyCharacters = std::max(maxKeyCharacters, strlen(entry.key.name));
    }
    char formatString[32];
    snprintf(formatString, sizeof(formatString), "%%-%zus ", maxKeyCharacters);

    bool first = true;
    char buffer[2048];
    StringAppender appender(buffer, sizeof(buffer));
    for (const auto& entry : attributes) {
            if(!first) appender.snprintf("\n");
            first = false;
            appender.snprintf(formatString, entry.key.name);
            m_AttributeDisplay.snprintf(entry, appender);
    }
    drawText(m_GlyphRenderer, context.viewport, buffer, 50, 50, 1, 1);
}

} /* namespace duke */
