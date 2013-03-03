/*
 * ImageRendere.h
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef IMAGERENDERE_H_
#define IMAGERENDERE_H_

namespace duke {

class Mesh;
struct Context;
struct ShaderPool;

void renderWithBoundTexture(const ShaderPool &shaderPool, const Mesh *pMesh, const Context &context);

} /* namespace duke */
#endif /* IMAGERENDERE_H_ */
