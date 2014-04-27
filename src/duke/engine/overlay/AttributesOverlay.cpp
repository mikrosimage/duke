#include "AttributesOverlay.hpp"
#include <duke/attributes/AttributeDisplay.hpp>
#include <duke/attributes/AttributesView.hpp>
#include <duke/engine/Context.hpp>
#include <duke/engine/rendering/GlyphRenderer.hpp>
#include <duke/imageio/PackedFrameDescriptionAndAttributes.hpp>
#include <algorithm>

namespace duke {

AttributesOverlay::AttributesOverlay(const GlyphRenderer& glyphRenderer) : m_GlyphRenderer(glyphRenderer) {}

void AttributesOverlay::render(const Context& context) const {
  if (!context.pCurrentImage) return;

  attribute::AttributesView attributeView;
  attributeView.merge(context.pCurrentImage->attributes);

  attribute::Attributes additional;
  const auto& description = context.pCurrentImage->description;
  additional.emplace_back("dk:frame resolution", asSlice<uint64_t>({description.width, description.height}));
  attributeView.merge(additional);

  attributeView.sort();

  // Lines are displayed from bottom to top.
  // Inverting attribute order.
  std::reverse(attributeView.begin(), attributeView.end());

  size_t maxKeyCharacters = 0;
  for (const auto* pEntry : attributeView) {
    maxKeyCharacters = std::max(maxKeyCharacters, strlen(pEntry->name));
  }

  bool first = true;
  BufferStringAppender<2048> buffer;
  for (const auto* pEntry : attributeView) {
    if (!first) buffer.append('\n');
    first = false;
    buffer.append(pEntry->name);
    const size_t left = maxKeyCharacters - strlen(pEntry->name) + 1;
    for (size_t i = 0; i < left; ++i) buffer.append(' ');
    attribute::append(*pEntry, buffer);
  }
  drawText(m_GlyphRenderer, context.viewport, buffer.c_str(), 50, 50, 1, 1);
}

} /* namespace duke */
