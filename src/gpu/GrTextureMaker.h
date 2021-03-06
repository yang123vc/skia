/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrTextureMaker_DEFINED
#define GrTextureMaker_DEFINED

#include "GrTextureProducer.h"

/**
 * Base class for sources that start out as something other than a texture (encoded image,
 * picture, ...).
 */
class GrTextureMaker : public GrTextureProducer {
public:
    /**
     *  Returns a texture that is safe for use with the params. If the size of the returned texture
     *  does not match width()/height() then the contents of the original must be scaled to fit
     *  the texture. Places the color space of the texture in (*texColorSpace).
     */
    GrTexture* refTextureForParams(const GrSamplerParams&, SkDestinationSurfaceColorMode,
                                   sk_sp<SkColorSpace>* texColorSpace);

    sk_sp<GrFragmentProcessor> createFragmentProcessor(
                                const SkMatrix& textureMatrix,
                                const SkRect& constraintRect,
                                FilterConstraint filterConstraint,
                                bool coordsLimitedToConstraintRect,
                                const GrSamplerParams::FilterMode* filterOrNullForBicubic,
                                SkColorSpace* dstColorSpace,
                                SkDestinationSurfaceColorMode) override;

protected:
    GrTextureMaker(GrContext* context, int width, int height, bool isAlphaOnly)
        : INHERITED(width, height, isAlphaOnly)
        , fContext(context) {}

    /**
     *  Return the maker's "original" texture. It is the responsibility of the maker to handle any
     *  caching of the original if desired.
     */
    virtual GrTexture* refOriginalTexture(bool willBeMipped, SkDestinationSurfaceColorMode) = 0;

    /**
     *  Returns the color space of the maker's "original" texture, assuming it was retrieved with
     *  the same destination color mode.
     */
    virtual sk_sp<SkColorSpace> getColorSpace(SkDestinationSurfaceColorMode) = 0;

    /**
     *  Return a new (uncached) texture that is the stretch of the maker's original.
     *
     *  The base-class handles general logic for this, and only needs access to the following
     *  method:
     *  - refOriginalTexture()
     *
     *  Subclass may override this if they can handle creating the texture more directly than
     *  by copying.
     */
    virtual GrTexture* generateTextureForParams(const CopyParams&, bool willBeMipped,
                                                SkDestinationSurfaceColorMode);

    GrContext* context() const { return fContext; }

private:
    GrContext*  fContext;

    typedef GrTextureProducer INHERITED;
};

#endif
