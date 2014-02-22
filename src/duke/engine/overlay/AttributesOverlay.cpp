#include "AttributesOverlay.hpp"
#include <duke/attributes/AttributeDisplay.hpp>
#include <duke/engine/Context.hpp>
#include <duke/engine/rendering/GlyphRenderer.hpp>
#include <duke/imageio/PackedFrameDescriptionAndAttributes.hpp>
#include <algorithm>

namespace duke {

namespace {

std::string getAttributes(const Attributes& attributes) {
    if (attributes.size() == 0) return {};
    std::string buffer;
    buffer.reserve(256);
    for (const auto& entry : attributes) {
        if(!buffer.empty()) buffer += '\n';
        buffer += nameString(entry);
        buffer += ' ';
        buffer += dataString(entry);
    }
    return buffer;
}

}  // namespace

AttributesOverlay::AttributesOverlay(const GlyphRenderer &glyphRenderer) :
                m_GlyphRenderer(glyphRenderer) {
}

void AttributesOverlay::render(const Context &context) const {
    if (!context.pCurrentImage) return;

    const Attributes& attributes = context.pCurrentImage->attributes;
    const std::string text = getAttributes(attributes);
    drawText(m_GlyphRenderer, context.viewport, text.c_str(), 50, 50, 1, 1);
}

} /* namespace duke */
