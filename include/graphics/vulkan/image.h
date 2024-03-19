//
// Created by Sam on 2024-02-09.
//

#ifndef DOUGHNUT_IMAGE_H
#define DOUGHNUT_IMAGE_H

#include "context.h"
#include "graphics/texture.h"
#include "staging_buffer.h"
#include "core/late_init.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    vk::Format findSupportedFormat(vk::PhysicalDevice physicalDevice,
                                   const std::vector<vk::Format> &candidates,
                                   vk::ImageTiling tiling,
                                   vk::FormatFeatureFlags features);

    vk::Format findDepthFormat(vk::PhysicalDevice physicalDevice);

    struct ImageConfiguration {
        vk::Extent2D extent;
        bool isDepthImage;
        bool isTextureImage;
        bool isTransferDestination;
        bool hasAlpha;
    };

    class Image {
    public:
        Image(Context &context,
              ImageConfiguration config);

        Image(Context &instance,
              vk::Image image,
              vk::Format format,
              vk::DeviceMemory memory);

        Image(Image &&other) noexcept;

        ~Image();

        vk::Image mImage;
        vk::DeviceMemory mMemory;
        vk::Format mFormat;
        vk::ImageUsageFlags mUsageFlags;

    private:
        Context &mContext;
        bool mLocallyConstructed;
    };
}

#endif //DOUGHNUT_IMAGE_H
