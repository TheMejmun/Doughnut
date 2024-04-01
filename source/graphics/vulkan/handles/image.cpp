//
// Created by Sam on 2024-02-09.
//

#include "graphics/vulkan/handles/image.h"
#include "graphics/vulkan/memory.h"
#include "io/logger.h"
#include "util/require.h"

using namespace dn;
using namespace dn::vulkan;

vk::Format vulkan::findSupportedFormat(vk::PhysicalDevice physicalDevice,
                                       const std::vector<vk::Format> &candidates,
                                       vk::ImageTiling tiling,
                                       vk::FormatFeatureFlags features) {
    for (vk::Format format: candidates) {
        vk::FormatProperties props = physicalDevice.getFormatProperties(format);

        if ((tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) ||
            (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format!");
}

vk::Format vulkan::findDepthFormat(vk::PhysicalDevice physicalDevice) {
    return findSupportedFormat(
            physicalDevice,
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal, // -> More efficient https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImageTiling.html
            vk::FormatFeatureFlagBits::eDepthStencilAttachment    // -> Can be used as depth/stencil attachment & input attachment
    );
}

ImageConfiguration constructConfiguration(vk::Extent2D extent, vk::Format format) {
    // TODO
    return {
            extent,
            false,
            false,
            false,
            false
    };
};

Image::Image(Context &context,
             vk::Image image,
             vk::Extent2D extent,
             vk::Format format,
             vk::DeviceMemory memory)
        : Handle<vk::Image, ImageConfiguration>(context, constructConfiguration(extent, format)),
          mMemory(memory),
          mFormat(format),
          mUsageFlags(),
          mLocallyConstructed(false) {
    mVulkan = image;
}

Image::Image(Context &context,
             ImageConfiguration config)
        : Handle<vk::Image, ImageConfiguration>(context, config),
          mLocallyConstructed(true) {

    if (config.isTextureImage) {
        // TODO format = config.hasAlpha ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8Srgb;
        mFormat = vk::Format::eR8G8B8A8Srgb;
    } else if (config.isDepthImage) {
        mFormat = findDepthFormat(mContext.mPhysicalDevice);
    }

    if (config.isTextureImage) {
        mUsageFlags |= vk::ImageUsageFlagBits::eSampled;
    } else if (config.isDepthImage) {
        mUsageFlags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    }

    if (config.isTransferDestination) {
        mUsageFlags |= vk::ImageUsageFlagBits::eTransferDst;
    }

    vk::ImageCreateInfo createInfo{
            {},
            vk::ImageType::e2D,
            mFormat,
            vk::Extent3D{config.extent.width, config.extent.height, 1},
            1,
            1,
            vk::SampleCountFlagBits::e1,
            vk::ImageTiling::eOptimal,
            mUsageFlags,
            vk::SharingMode::eExclusive,
            0, // Ignored if not concurrent
            nullptr, // Ignored if not concurrent
            vk::ImageLayout::eUndefined
    };

    mVulkan = mContext.mDevice.createImage(createInfo);
    vk::MemoryRequirements memoryRequirements = mContext.mDevice.getImageMemoryRequirements(mVulkan);

    vk::MemoryPropertyFlags properties{vk::MemoryPropertyFlagBits::eDeviceLocal};

    vk::MemoryAllocateInfo allocateInfo{
            memoryRequirements.size,
            findMemoryType(mContext.mPhysicalDevice, memoryRequirements.memoryTypeBits, properties)
    };

    mMemory = mContext.mDevice.allocateMemory(allocateInfo);
    mContext.mDevice.bindImageMemory(mVulkan, mMemory, 0);
}

Image::~Image() {
    if (mLocallyConstructed) {
        if (mVulkan != nullptr) { mContext.mDevice.destroy(mVulkan); }
        if (mMemory != nullptr) { mContext.mDevice.free(mMemory); }
    }
}