/*
 * RenderPass.h
 *
 *  Created on: Dec 4, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef RENDERPASS_H_
#define RENDERPASS_H_

#include <duke/utils/NonCopyable.h>

struct RenderPass: public NonCopyable {
public:
	RenderPass();
	~RenderPass();
};

#endif /* RENDERPASS_H_ */
