//
// Created by Sam on 2024-02-22.
//

#include "graphics/vulkan/handles/sampler.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

Sampler::Sampler(Context &context, const SamplerConfiguration &config)
        : Handle<vk::Sampler, SamplerConfiguration>(context, config) {

    vk::SamplerAddressMode addressMode;
    switch (config.edgeMode) {
        case REPEAT:
            addressMode = vk::SamplerAddressMode::eRepeat;
            break;
        case MIRRORED_REPEAT:
            addressMode = vk::SamplerAddressMode::eMirroredRepeat;
            break;
        case CLAMP:
            addressMode = vk::SamplerAddressMode::eClampToEdge;
            break;
        case MIRROR_CLAMP:
            addressMode = vk::SamplerAddressMode::eMirrorClampToEdge;
            break;
        case SOLID_WHITE:
        case SOLID_BLACK :
            addressMode = vk::SamplerAddressMode::eClampToBorder;
            break;
    }

    auto anisotropy = mContext.mPhysicalDevice.getProperties().limits.maxSamplerAnisotropy;

    log::v("Max anisotropy", anisotropy);

    vk::SamplerCreateInfo samplerInfo{
            {},
            vk::Filter::eNearest, // TODO check if eCubicEXT needs some extension to be enabled
            vk::Filter::eNearest, // TODO check if eCubicEXT needs some extension to be enabled
            vk::SamplerMipmapMode::eLinear,
            addressMode, addressMode, addressMode,
            0.0f,
            mContext.mOptionalFeatures.supportsAnisotropicFiltering,
            anisotropy,
            vk::False,
            vk::CompareOp::eNever, // For percentage-closer filtering
            0.0f,
            0.0f,
            (config.edgeMode == SOLID_WHITE) ? vk::BorderColor::eFloatOpaqueWhite : vk::BorderColor::eFloatOpaqueBlack,
            vk::False
    };

    mVulkan = mContext.mDevice.createSampler(samplerInfo);
}

Sampler::~Sampler() {
    if (mVulkan != nullptr) mContext.mDevice.destroy(mVulkan);
}