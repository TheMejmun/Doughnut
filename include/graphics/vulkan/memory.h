//
// Created by Sam on 2024-02-09.
//

#ifndef DOUGHNUT_MEMORY_H
#define DOUGHNUT_MEMORY_H

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    uint32_t findMemoryType(vk::PhysicalDevice physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
}

#endif //DOUGHNUT_MEMORY_H
