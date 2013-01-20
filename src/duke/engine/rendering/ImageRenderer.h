/*
 * ImageRendere.h
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef IMAGERENDERE_H_
#define IMAGERENDERE_H_

struct Attributes;

namespace duke {

class ITexture;
class Mesh;
struct Context;

void render(const Mesh *pMesh,const ITexture& texture, const Attributes &attributes, const Context &context);

} /* namespace duke */
#endif /* IMAGERENDERE_H_ */
