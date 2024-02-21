//
// Created by Sam on 2024-02-09.
//

#ifndef DOUGHNUT_IMAGE_H
#define DOUGHNUT_IMAGE_H

#include "instance.h"
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
        Image(Instance &instance,
              ImageConfiguration config);

        Image(Instance &instance,
              vk::Image image,
              vk::DeviceMemory memory);

        Image(Image &&other) noexcept;

        ~Image();

        void upload(const Texture &texture);

        bool isCurrentlyUploading();

        void awaitUpload();

        vk::Image mImage;
        vk::DeviceMemory mMemory;
        vk::Format mFormat{};
        vk::ImageUsageFlags mUsageFlags{};

    private:
        Instance &mInstance;
        bool mLocallyConstructed;
    };
}

#endif //DOUGHNUT_IMAGE_H
