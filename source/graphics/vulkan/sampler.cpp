//
// Created by Sam on 2024-02-22.
//

#include "graphics/vulkan/sampler.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

Sampler::Sampler(dn::vulkan::Instance &instance, const dn::vulkan::SamplerConfiguration &config)
        : mInstance(instance) {
    log::d("Creating Sampler");

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

    auto anisotropy = mInstance.mPhysicalDevice.getProperties().limits.maxSamplerAnisotropy;

    log::v("Max anisotropy", anisotropy);

    vk::SamplerCreateInfo samplerInfo{
            {},
            vk::Filter::eNearest, // TODO check if eCubicEXT needs some extension to be enabled
            vk::Filter::eNearest, // TODO check if eCubicEXT needs some extension to be enabled
            vk::SamplerMipmapMode::eLinear,
            addressMode, addressMode, addressMode,
            0.0f,
            mInstance.mOptionalFeatures.supportsAnisotropicFiltering,
            anisotropy,
            vk::False,
            vk::CompareOp::eNever, // For percentage-closer filtering
            0.0f,
            0.0f,
            (config.edgeMode == SOLID_WHITE) ? vk::BorderColor::eFloatOpaqueWhite : vk::BorderColor::eFloatOpaqueBlack,
            vk::False
    };

    mSampler = mInstance.mDevice.createSampler(samplerInfo);
}

Sampler::~Sampler() {
    log::d("Destroying Sampler");
    mInstance.mDevice.destroy(mSampler);
}