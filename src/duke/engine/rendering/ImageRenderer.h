/*
 * ImageRendere.h
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

namespace duke {

class Mesh;
struct Context;
struct ShaderPool;

void renderWithBoundTexture(const ShaderPool &shaderPool, const Mesh *pMesh, const Context &context);

} /* namespace duke */
