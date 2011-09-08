/*
 * RenderingContext.h
 *
 *  Created on: 10 sept. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef RENDERINGCONTEXT_H_
#define RENDERINGCONTEXT_H_

#include "ITextureProvider.h"
#include "Image.h"
#include <duke_io/ImageDescription.h>
#include <vector>
#include <string>

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

typedef ::boost::shared_ptr<ITextureProvider> TexturePtr;
typedef ::boost::unordered_map<std::string, TexturePtr> RenderTargets;

typedef ::boost::shared_ptr<Image> ImagePtr;
typedef ::boost::unordered_map<std::string, ImagePtr> DumpedImages;

typedef std::vector<TexturePtr> Textures;

typedef std::vector<ImageDescription> ImageDescriptions;
typedef std::vector<std::string> Scopes;
typedef Scopes::const_iterator ScopesItr;
typedef Scopes::const_reverse_iterator ScopesRItr;

struct RenderingContext {
private:
    const ImageDescriptions * pImages;
    size_t frameCount;
    unsigned rtWidth;
    unsigned rtHeight;
public:
    Scopes scopes;
    DumpedImages dumpedImages;
    RenderTargets renderTargets;
    Textures textures;

    RenderingContext() :
        pImages(NULL), frameCount(0), rtWidth(0), rtHeight(0) {
        reset();
    }

    void reset() {
        pImages = NULL;
        frameCount = 0;
        rtWidth = 0;
        rtHeight = 0;
        scopes.clear();
        renderTargets.clear();
        textures.clear();
        dumpedImages.clear();
    }

    void set(const ImageDescriptions& images, size_t displayedFrameCount, unsigned renderTargetWidth, unsigned renderTargetHeight) {
        pImages = &images;
        displayedFrameCount = displayedFrameCount;
        setRenderTarget(renderTargetWidth, renderTargetHeight);
    }

    void setRenderTarget(unsigned renderTargetWidth, unsigned renderTargetHeight) {
        rtWidth = renderTargetWidth;
        rtHeight = renderTargetHeight;
    }

    inline const ImageDescriptions& images() const {
        assert(pImages);
        return *pImages;
    }

    inline unsigned renderTargetWidth() const {
        return rtWidth;
    }

    inline unsigned renderTargetHeight() const {
        return rtHeight;
    }

    inline unsigned displayedFrameCount() const {
        return frameCount;
    }
};

#endif /* RENDERINGCONTEXT_H_ */
