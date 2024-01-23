//
// Created by Saman on 24.08.23.
//

#ifndef DOUGHNUT_VULKAN_MEMORY_H
#define DOUGHNUT_VULKAN_MEMORY_H

#include "preprocessor.h"

#include <cstdint>
#include <vulkan/vulkan.h>

namespace Doughnut::Graphics::Vk::Memory {
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
}
#endif //DOUGHNUT_VULKAN_MEMORY_H
