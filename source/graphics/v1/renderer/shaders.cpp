//
// Created by Sam on 2023-04-11.
//

#include "graphics/v1/renderer.h"
#include "graphics/v1/vulkan/vulkan_devices.h"

using namespace Doughnut::Graphics;

VkShaderModule Renderer::createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    // Cast the pointer. Vectors already handle proper memory alignment.
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(Vk::Devices::logical, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module!");
    }

    return shaderModule;
}