#pragma once

#include "IOverlay.hpp"
#include <duke/attributes/AttributeDisplay.hpp>
#include <memory>

namespace duke {

struct Context;
struct GlyphRenderer;

class AttributesOverlay: public IOverlay {
public:
	AttributesOverlay(const GlyphRenderer&);
	virtual void render(const Context&) const;
private:
	const GlyphRenderer &m_GlyphRenderer;
	AttributeDisplay m_AttributeDisplay;
};

} /* namespace duke */
