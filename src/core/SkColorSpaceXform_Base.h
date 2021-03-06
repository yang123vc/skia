/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkColorSpaceXform_Base_DEFINED
#define SkColorSpaceXform_Base_DEFINED

#include "SkColorSpace.h"
#include "SkColorSpace_Base.h"
#include "SkColorSpaceXform.h"
#include "SkResourceCache.h"

class SkColorSpace_XYZ;

class SkColorSpaceXform_Base : public SkColorSpaceXform {
public:
    static constexpr int kDstGammaTableSize = 1024;

protected:
    virtual bool onApply(ColorFormat dstFormat, void* dst, ColorFormat srcFormat, const void* src,
                         int count, SkAlphaType alphaType) const = 0;

private:
    static void BuildDstGammaTables(const uint8_t* outGammaTables[3], uint8_t* gammaTableStorage,
                                    const SkColorSpace_XYZ* space, bool gammasAreMatching);

    friend class SkColorSpaceXform;
    friend class SkColorSpace_XYZ;
};

enum SrcGamma {
    kLinear_SrcGamma,
    kTable_SrcGamma,
};

enum DstGamma {
    kLinear_DstGamma,
    kSRGB_DstGamma,
    k2Dot2_DstGamma,
    kTable_DstGamma,
};

enum ColorSpaceMatch {
    kNone_ColorSpaceMatch,
    kGamut_ColorSpaceMatch,
    kFull_ColorSpaceMatch,
};

template <SrcGamma kSrc, DstGamma kDst, ColorSpaceMatch kCSM>
class SkColorSpaceXform_XYZ : public SkColorSpaceXform_Base {
protected:
    bool onApply(ColorFormat dstFormat, void* dst, ColorFormat srcFormat, const void* src,
                 int count, SkAlphaType alphaType) const override;

private:
    SkColorSpaceXform_XYZ(SkColorSpace_XYZ* srcSpace, const SkMatrix44& srcToDst,
                          SkColorSpace_XYZ* dstSpace);

    // Contain pointers into storage or pointers into precomputed tables.
    const float*              fSrcGammaTables[3];
    SkAutoTMalloc<float>      fSrcStorage;
    const uint8_t*            fDstGammaTables[3];
    sk_sp<SkData>             fDstStorage;

    float                     fSrcToDst[16];

    friend class SkColorSpaceXform;
    friend std::unique_ptr<SkColorSpaceXform> SlowIdentityXform(SkColorSpace_XYZ* space);
};

struct LoadTablesContext {
    const uint32_t* fSrc;
    const float*    fR;
    const float*    fG;
    const float*    fB;
};

struct StoreTablesContext {
    uint32_t*      fDst;
    const uint8_t* fR;
    const uint8_t* fG;
    const uint8_t* fB;
    int            fCount;
};

class SkColorSpaceXform_Pipeline : public SkColorSpaceXform_Base {
protected:
    virtual bool onApply(ColorFormat dstFormat, void* dst, ColorFormat srcFormat, const void* src,
                         int count, SkAlphaType alphaType) const;

private:
    SkColorSpaceXform_Pipeline(SkColorSpace_XYZ* srcSpace, const SkMatrix44& srcToDst,
                               SkColorSpace_XYZ* dstSpace, ColorSpaceMatch csm, SrcGamma srcGamma,
                               DstGamma dstGamma);

    // Contain pointers into storage or pointers into precomputed tables.
    const float*              fSrcGammaTables[3];
    SkAutoTMalloc<float>      fSrcStorage;
    const uint8_t*            fDstGammaTables[3];
    sk_sp<SkData>             fDstStorage;

    float                     fSrcToDst[12];

    ColorSpaceMatch           fCSM;
    SrcGamma                  fSrcGamma;
    DstGamma                  fDstGamma;

    friend class SkColorSpaceXform;
    friend std::unique_ptr<SkColorSpaceXform> SlowIdentityXform(SkColorSpace_XYZ* space);
};

// For testing.  Bypasses opts for when src and dst color spaces are equal.
std::unique_ptr<SkColorSpaceXform> SlowIdentityXform(SkColorSpace_XYZ* space);

#endif
