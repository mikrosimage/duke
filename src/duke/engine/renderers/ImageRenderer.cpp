/*
 * ImageRenderer.cpp
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#include "ImageRenderer.h"
#include <duke/engine/Context.h>
#include <duke/engine/LoadableTexture.h>

#include <duke/gl/GL.h>

namespace duke {


AbstractRenderer::AbstractRenderer(SharedVertexShader vs, SharedFragmentShader fs) :
		m_Program(vs, fs), gViewport(m_Program.getUniformLocation("gViewport")), //
		gImage(m_Program.getUniformLocation("gImage")), //
		gPan(m_Program.getUniformLocation("gPan")), //
		gTextureSampler(m_Program.getUniformLocation("rectangleImageSampler")), //
		m_pMesh(getSquare()) {
}

AbstractRenderer::~AbstractRenderer() {
}

static const char * const pPanScanVertexShader=R"(
#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;

uniform ivec2 gViewport;
uniform ivec2 gImage;
uniform ivec2 gPan;
uniform int gZoom;

out vec2 vVaryingTexCoord; 

mat4 ortho(int left, int right, int bottom, int top) {
	mat4 Result;
	Result[0][0] = float(2) / (right - left);
	Result[1][1] = float(2) / (top - bottom);
	Result[2][2] = - float(1);
	Result[3][3] = 1;
	Result[3][0] = - (right + left) / (right - left);
	Result[3][1] = - (top + bottom) / (top - bottom);
	return Result;
}

mat4 translate(mat4 m, vec3 v) {
	mat4 Result = mat4(m);
	Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
	return Result;
}

mat4 scale(mat4 m, vec3 v) {
	mat4 Result = mat4(m);
	Result[0] = m[0] * v[0];
	Result[1] = m[1] * v[1];
	Result[2] = m[2] * v[2];
	Result[3] = m[3];
	return Result;
}

void main() {
	ivec2 translating = ivec2(0); // translation must be integer to prevent aliasing
	translating += gViewport; // moving to center
	translating /= 2; // moving to center
	translating += gPan; // moving to center
	vec2 scaling = vec2(1);
	scaling /= 2; // bringing square from [-1,1] to [-.5,.5]
	scaling *= gImage; // to pixel dimension
	scaling *= (1 + (0.1 * gZoom)); // zoom
	mat4 world = mat4(1);
	world = translate(world, vec3(translating, 0)); // move to center
	world = scale(world, vec3(scaling, 1));
	mat4 proj = ortho(0, gViewport.x, 0, gViewport.y);
	mat4 worldViewProj = proj * world;
	gl_Position = worldViewProj * vec4(Position, 1.0);
	vVaryingTexCoord = UV * abs(gImage);
})";

static const char * const pGradingFragmentShader=R"(
#version 330

out vec4 vFragColor;
uniform sampler2DRect rectangleImageSampler;
uniform bvec4 gShowChannel;
uniform float gExposure;
smooth in vec2 vVaryingTexCoord;

void main(void)
{
	vec4 sampled = texture(rectangleImageSampler, vVaryingTexCoord);
	if(any(gShowChannel.xyz))
		sampled *= vec4(gShowChannel.xyz,1);
	if(gShowChannel.w)
		sampled = vec4(sampled.aaa,1);
	vFragColor =  sampled * gExposure;
})";

ImageRenderer::ImageRenderer() :
		AbstractRenderer(makeVertexShader(pPanScanVertexShader), makeFragmentShader(pGradingFragmentShader)), //
		gZoom(m_Program.getUniformLocation("gZoom")), //
		gShowChannel(m_Program.getUniformLocation("gShowChannel")), //
		gExposure(m_Program.getUniformLocation("gExposure")) {
}

void ImageRenderer::draw(const duke::Context &context, const LoadableTexture& texture) const {
	m_Program.use();
	glUniform2i(gViewport, context.viewport.dimension.x, context.viewport.dimension.y);
	glUniform1i(gTextureSampler, 0);
	const auto scopeBind = texture.use(gImage);
	glUniform2i(gPan, context.pan.x, context.pan.y);
	glUniform1i(gZoom, context.zoom);
	glUniform1f(gExposure, context.exposure);
	glUniform4i(gShowChannel, context.channels.x, context.channels.y, context.channels.z, context.channels.w);
	m_pMesh->draw();
	checkError();
}

} /* namespace duke */
