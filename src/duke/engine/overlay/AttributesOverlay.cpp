#include "AttributesOverlay.hpp"
#include <duke/attributes/AttributeDisplay.hpp>
#include <duke/attributes/AttributesView.hpp>
#include <duke/engine/Context.hpp>
#include <duke/engine/rendering/GlyphRenderer.hpp>
#include <duke/imageio/PackedFrameDescriptionAndAttributes.hpp>
#include <algorithm>

namespace duke {

AttributesOverlay::AttributesOverlay(const GlyphRenderer &glyphRenderer) :
                m_GlyphRenderer(glyphRenderer) {
}

void AttributesOverlay::render(const Context &context) const {
    if (!context.pCurrentImage) return;

    attribute::AttributesView attributeView;
    attributeView.merge(context.pCurrentImage->attributes);
    attributeView.sort();

    size_t maxKeyCharacters = 0;
    for (const auto& entry : attributeView) {
    	maxKeyCharacters = std::max(maxKeyCharacters, strlen(entry.name));
    }

    bool first = true;
    BufferStringAppender<2048> buffer;
    for (const auto& entry : attributeView) {
            if(!first) buffer.append('\n');
            first = false;
            buffer.append(entry.name);
            const size_t left = maxKeyCharacters - strlen(entry.name) + 1;
            for (size_t i = 0; i < left; ++i) buffer.append(' ');
            attribute::append(entry, buffer);
    }
    drawText(m_GlyphRenderer, context.viewport, buffer.c_str(), 50, 50, 1, 1);
}

} /* namespace duke */
