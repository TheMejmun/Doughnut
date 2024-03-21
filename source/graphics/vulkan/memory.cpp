//
// Created by Sam on 2024-02-09.
//

#include "graphics/vulkan/memory.h"
#include "util/require.h"

using namespace dn;
using namespace dn::vulkan;

uint32_t vulkan::findMemoryType(vk::PhysicalDevice physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPhysicalDeviceMemoryProperties.html
        // Look for first memory of type with required properties (-> Fastest)
        const bool isRequiredMemoryType = (typeFilter & (1 << i));
        const bool hasRequiredProperties = (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties;
        if (isRequiredMemoryType && hasRequiredProperties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}