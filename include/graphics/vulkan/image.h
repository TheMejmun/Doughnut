//
// Created by Sam on 2024-02-09.
//

#ifndef DOUGHNUTSANDBOX_IMAGE_H
#define DOUGHNUTSANDBOX_IMAGE_H

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    vk::Format findSupportedFormat(vk::PhysicalDevice physicalDevice,
                                   const std::vector<vk::Format> &candidates,
                                   vk::ImageTiling tiling,
                                   vk::FormatFeatureFlags features);

    vk::Format findDepthFormat(vk::PhysicalDevice physicalDevice);

    struct ImageConfiguration {
        uint32_t width;
        uint32_t height;
        vk::Format format;
        vk::ImageTiling tiling;
        vk::ImageUsageFlags usage;
        vk::MemoryPropertyFlags properties;
    };

    class Image {
    public:
        Image(vk::Device device,
              vk::PhysicalDevice physicalDevice,
              ImageConfiguration config);

        Image(vk::Device device,
              vk::Image image,
              vk::DeviceMemory memory);

        Image(Image &&other)  noexcept ;

        ~Image();

        vk::Image mImage;
        vk::DeviceMemory mMemory;
    private:
        vk::Device mDevice;
        bool mLocallyConstructed;
    };
}

#endif //DOUGHNUTSANDBOX_IMAGE_H
