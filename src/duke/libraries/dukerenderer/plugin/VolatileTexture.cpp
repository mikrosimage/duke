#include "VolatileTexture.h"
#include "IFactory.h"
#include "ITextureBase.h"
#include <dukeio/ImageDescription.h>
#include <iostream>
#include <cassert>

VolatileTexture::VolatileTexture(IFactory& factory, const ImageDescription& description, const unsigned flags) :
    m_Description(description) {
    const PoolRequest request(description.format, flags && TEX_MIPMAP > 0, description.width, description.height);

    m_pTexture = factory.getTexturePool().get(request);
    if (!m_pTexture) {
        const ResourcePtr pTexture(factory.createTexture(description, flags));
        assert( pTexture );
        m_pTexture = factory.getTexturePool().putAndGet(request, pTexture);
    }
    assert( getTexture() );
    if (!getTexture()->isRenderTarget()) // image not to be render to ? we must update it
        getTexture()->update(description, reinterpret_cast<const unsigned char*> (description.pImageData));
}

VolatileTexture::~VolatileTexture() {
}

ITextureBase* VolatileTexture::getTexture() const {
    return m_pTexture ? m_pTexture->getTexture<ITextureBase> () : NULL;
}

const ImageDescription& VolatileTexture::getImageDescription() const {
    return m_Description;
}

