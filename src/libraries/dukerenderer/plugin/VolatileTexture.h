/*
 * VolatileTexture.h
 *
 *  Created on: 1 juin 2010
 *      Author: Guillaume Chatelet
 */

#ifndef VOLATILETEXTURE_H_
#define VOLATILETEXTURE_H_

#include "ITextureProvider.h"
#include "TexturePool.h"
#include <string>

class IFactory;

class VolatileTexture : public ITextureProvider {
public:
    VolatileTexture(IFactory& factory, const ImageDescription& spec, const unsigned flags = 0);
    virtual ~VolatileTexture();

    virtual ITextureBase* getTexture() const;
    virtual const ImageDescription& getImageDescription() const;

private:
    ImageDescription m_Description;
    TexturePool::ScopedTexturePtr m_pTexture;
};

#endif /* VOLATILETEXTURE_H_ */
