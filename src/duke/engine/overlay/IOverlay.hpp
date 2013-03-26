#pragma once

#include <duke/NonCopyable.hpp>

namespace duke {

struct Context;
class IOverlay: public noncopyable {
public:
	virtual ~IOverlay() = 0;
	virtual void render(const Context&) const =0;
};

} /* namespace duke */
