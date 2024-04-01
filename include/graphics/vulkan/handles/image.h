//
// Created by Sam on 2024-02-09.
//

#ifndef DOUGHNUT_IMAGE_H
#define DOUGHNUT_IMAGE_H

#include "graphics/vulkan/context.h"
#include "graphics/texture.h"
#include "graphics/vulkan/staging_buffer.h"
#include "core/late_init.h"
#include "graphics/vulkan/handle.h"

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

    class Image : public Handle<vk::Image, ImageConfiguration> {
    public:
        Image(Context &context,
              ImageConfiguration config);

        Image(Image &&) = default;

        Image(Context &instance,
              vk::Image image,
              vk::Extent2D extent,
              vk::Format format,
              vk::DeviceMemory memory);

        ~Image();

        vk::DeviceMemory mMemory;
        vk::Format mFormat;
        vk::ImageUsageFlags mUsageFlags;
        const bool mLocallyConstructed;
    };
}

#endif //DOUGHNUT_IMAGE_H
