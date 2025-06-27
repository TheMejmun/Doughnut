//
// Created by Sam on 2024-02-11.
//

#include "graphics/vulkan/handles/descriptor_set_layout.h"
#include "util/require.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

DescriptorSetLayout::DescriptorSetLayout(Context &context, const DescriptorSetLayoutConfiguration &config)
        : Handle<vk::DescriptorSetLayout, DescriptorSetLayoutConfiguration>(context, config){
    // TODO actually take into account the configuration

    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings{};
    layoutBindings.emplace_back(
            0,
            vk::DescriptorType::eUniformBuffer,
            1,
            vk::ShaderStageFlagBits::eVertex, // Can also be all shader stages: VK_SHADER_STAGE_ALL_GRAPHICS
            nullptr // Relevant for image sampling
    );
    layoutBindings.emplace_back(
            1,
            vk::DescriptorType::eCombinedImageSampler,
            1,
            vk::ShaderStageFlagBits::eFragment, // Can also be all shader stages: VK_SHADER_STAGE_ALL_GRAPHICS
            nullptr // Relevant for image sampling
    );

    vk::DescriptorSetLayoutCreateInfo createInfo{
            {},
            static_cast<uint32_t>(layoutBindings.size()),
            layoutBindings.data()
    };

    mVulkan = mContext.mDevice.createDescriptorSetLayout(createInfo);
}

DescriptorSetLayout::~DescriptorSetLayout() {
    if (mVulkan != nullptr) { mContext.mDevice.destroy(mVulkan); }
}