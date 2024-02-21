//
// Created by Sam on 2024-02-09.
//

#include "graphics/vulkan/image.h"
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

Image::Image(Instance &instance, vk::Image image, vk::DeviceMemory memory)
        : mInstance(instance), mImage(image), mMemory(memory), mLocallyConstructed(false) {
    log::v("Creating Image from existing vk::Image");
}

Image::Image(Instance &instance,
             ImageConfiguration config)
        : mInstance(instance), mLocallyConstructed(true) {
    log::v("Creating Image");

    if (config.isTextureImage) {
        // TODO format = config.hasAlpha ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8Srgb;
        mFormat = vk::Format::eR8G8B8A8Srgb;
    } else if (config.isDepthImage) {
        mFormat = findDepthFormat(mInstance.mPhysicalDevice);
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

    mImage = mInstance.mDevice.createImage(createInfo);
    vk::MemoryRequirements memoryRequirements = mInstance.mDevice.getImageMemoryRequirements(mImage);

    vk::MemoryPropertyFlags properties{vk::MemoryPropertyFlagBits::eDeviceLocal};

    vk::MemoryAllocateInfo allocateInfo{
            memoryRequirements.size,
            findMemoryType(mInstance.mPhysicalDevice, memoryRequirements.memoryTypeBits, properties)
    };

    mMemory = mInstance.mDevice.allocateMemory(allocateInfo);
    mInstance.mDevice.bindImageMemory(mImage, mMemory, 0);
}

Image::Image(dn::vulkan::Image &&other) noexcept
        : mImage(std::exchange(other.mImage, nullptr)),
          mMemory(std::exchange(other.mMemory, nullptr)),
          mInstance(other.mInstance),
          mLocallyConstructed(other.mLocallyConstructed) {
    log::v("Moving Image");
}

//void Image::upload(const dn::Texture &texture) {
//    mStagingBuffer.emplace(
//            mInstance,
//            StagingBufferConfiguration{}
//    );
    // TODO
//    mStagingBuffer->upload(
//            static_cast<uint32_t>( texture.size()),
//            texture.mData,
//            target,
//            0
//    );
//}

//void Image::awaitUpload() {
//    mStagingBuffer.reset();
//}
//
//bool Image::isCurrentlyUploading() {
//    return mStagingBuffer.has_value() && mStagingBuffer->isCurrentlyUploading();
//}

Image::~Image() {
    if (mLocallyConstructed) {
        log::v("Destroying Image");
        if (mImage != nullptr) { mInstance.mDevice.destroy(mImage); }
        if (mMemory != nullptr) { mInstance.mDevice.free(mMemory); }
    } else {
        log::v("Skipping Image destruction");
    }
}