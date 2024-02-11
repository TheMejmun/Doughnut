//
// Created by Sam on 2024-02-11.
//

#include "graphics/vulkan/descriptor_set.h"
#include "util/require.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

DescriptorSetLayout::DescriptorSetLayout(Instance &instance, dn::vulkan::DescriptorSetLayoutConfiguration config)
        : mInstance(instance) {
    log::d("Creating DescriptorSetLayout");
    require(config.descriptorSetConfigs.size() == 1, "TODO implement multiple descriptor bindings");

    // TODO actually take into account the configuration
    vk::DescriptorSetLayoutBinding layoutBinding0{
            0,
            vk::DescriptorType::eUniformBuffer,
            1,
            vk::ShaderStageFlagBits::eVertex, // Can also be all shader stages: VK_SHADER_STAGE_ALL_GRAPHICS
            nullptr // Relevant for image sampling
    };

    vk::DescriptorSetLayoutCreateInfo createInfo{
            {},
            1,
            &layoutBinding0
    };

    mLayout = mInstance.mDevice.createDescriptorSetLayout(createInfo);
}

DescriptorSetLayout::DescriptorSetLayout(dn::vulkan::DescriptorSetLayout &&other) noexcept
        : mLayout(std::exchange(other.mLayout, nullptr)), mInstance(other.mInstance) {
    log::d("Moving DescriptorSetLayout");
}

DescriptorSetLayout::~DescriptorSetLayout() {
    log::d("Destroying DescriptorSetLayout");
    if (mLayout != nullptr) { mInstance.mDevice.destroy(mLayout); }
}