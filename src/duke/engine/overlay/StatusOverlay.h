/*
 * StatusOverlay.h
 *
 *  Created on: Feb 19, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef STATUSOVERLAY_H_
#define STATUSOVERLAY_H_

#include "IOverlay.h"
#include <duke/time/FrameUtils.h>
#include <string>
#include <memory>

namespace duke {

struct Context;
struct GlyphRenderer;

class StatusOverlay: public duke::IOverlay {
public:
	StatusOverlay(const std::shared_ptr<GlyphRenderer>&);

	void setString(const Time& time, const std::string&msg);

	virtual void render(const Context&) const;
private:
	const std::shared_ptr<GlyphRenderer> m_pGlyphRenderer;
	Time m_ShowTime;
	std::string m_Message;
};

} /* namespace duke */
#endif /* STATUSOVERLAY_H_ */
