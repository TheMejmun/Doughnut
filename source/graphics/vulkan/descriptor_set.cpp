//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/descriptor_set.h"
#include "util/require.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

DescriptorSet::DescriptorSet(Instance &instance,
                             DescriptorSetLayout &layout,
                             DescriptorPool &pool,
                             const DescriptorSetConfiguration &config)
        : mInstance(instance), mConfig(config) {
    log::d("Creating DescriptorSet");

    std::vector<vk::DescriptorSetLayout> layouts(config.setCount, layout.mLayout);
    vk::DescriptorSetAllocateInfo allocInfo{
            pool.mDescriptorPool,
            static_cast<uint32_t>(layouts.size()),
            layouts.data()
    };

    mDescriptorSets = mInstance.mDevice.allocateDescriptorSets(allocInfo);
    mBufferIndices.resize(mDescriptorSets.size());

    for (size_t i = 0; i < config.setCount; i++) {

        std::vector<vk::WriteDescriptorSet> descriptorWrites{};

        const auto reserveResult = mConfig.uboBuffer.reserve(config.uboSize);
        require(!reserveResult.notEnoughSpace, "Could not allocate descriptor set - Buffer too small!");
        vk::DescriptorBufferInfo bufferInfo{
                mConfig.uboBuffer.mBuffer,
                reserveResult.position.memoryIndex,
                config.uboSize
        };

        descriptorWrites.emplace_back(
                mDescriptorSets[i],
                0,
                0,
                1,
                vk::DescriptorType::eUniformBuffer,
                nullptr,
                &bufferInfo,
                nullptr
        );

        vk::DescriptorImageInfo imageInfo{
                mConfig.sampler.mSampler,
                mConfig.imageView.mImageView,
                vk::ImageLayout::eShaderReadOnlyOptimal,
        };

        descriptorWrites.emplace_back(
                mDescriptorSets[i],
                1,
                0,
                1,
                vk::DescriptorType::eCombinedImageSampler,
                &imageInfo,
                nullptr,
                nullptr
        );

        mInstance.mDevice.updateDescriptorSets(descriptorWrites, nullptr);
    }
}

DescriptorSet::DescriptorSet(dn::vulkan::DescriptorSet &&other) noexcept
        : mInstance(other.mInstance),
          mConfig(other.mConfig),
          mDescriptorSets(std::exchange(other.mDescriptorSets, {})),
          mBufferIndices(std::exchange(other.mBufferIndices, {})) {
    log::d("Moving DescriptorSet");
}

DescriptorSet::~DescriptorSet() {
    log::d("Destroying DescriptorSet");

    // Descriptor Sets automatically cleared by Pool destruction?
}