//
// Created by Saman on 24.08.23.
//

#ifndef DOUGHNUT_VULKAN_IMAGES_H
#define DOUGHNUT_VULKAN_IMAGES_H

#include "preprocessor.h"

#include <vulkan/vulkan.h>

namespace dn::vulkan::Images {
    void createTextureImage();

    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);

    VkImageView
    createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);
}

#endif //DOUGHNUT_VULKAN_IMAGES_H
