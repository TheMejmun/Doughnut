//
// Created by Sam on 2024-02-09.
//

#include "graphics/vulkan/image.h"
#include "graphics/vulkan/memory.h"
#include "io/logger.h"

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

Image::Image(vk::Device device, vk::Image image, vk::DeviceMemory memory)
        : mDevice(device), mImage(image), mMemory(memory), mLocallyConstructed(false) {
    log::v("Creating Image from existing vk::Image");
}

Image::Image(vk::Device device,
             vk::PhysicalDevice physicalDevice,
             ImageConfiguration config)
        : mDevice(device), mLocallyConstructed(true) {
    log::v("Creating Image");

    vk::ImageCreateInfo createInfo{
            {},
            vk::ImageType::e2D,
            config.format,
            vk::Extent3D{config.width, config.height, 1},
            1,
            1,
            vk::SampleCountFlagBits::e1,
            config.tiling,
            config.usage,
            vk::SharingMode::eExclusive,
            0, // Ignored if not concurrent
            nullptr, // Ignored if not concurrent
            vk::ImageLayout::eUndefined
    };

    mImage = mDevice.createImage(createInfo);
    vk::MemoryRequirements memoryRequirements = mDevice.getImageMemoryRequirements(mImage);

    vk::MemoryAllocateInfo allocateInfo{
            memoryRequirements.size,
            findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, config.properties)
    };

    mMemory = mDevice.allocateMemory(allocateInfo);
    mDevice.bindImageMemory(mImage, mMemory, 0);
}

Image::Image(dn::vulkan::Image &&other) noexcept
        : mImage(std::exchange(other.mImage, nullptr)),
          mMemory(std::exchange(other.mMemory, nullptr)),
          mDevice(other.mDevice),
          mLocallyConstructed(other.mLocallyConstructed) {
    log::v("Moving Image");
}

Image::~Image() {
    if (mLocallyConstructed) {
        log::v("Destroying Image");
        if (mImage != nullptr) { mDevice.destroy(mImage); }
        if (mMemory != nullptr) { mDevice.free(mMemory); }
    } else {
        log::v("Skipping Image destruction");
    }
}