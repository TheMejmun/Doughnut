//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/descriptor_set.h"
#include "util/require.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

DescriptorSet::DescriptorSet(Instance &instance,
                             Buffer &uboBuffer,
                             DescriptorSetLayout &layout,
                             DescriptorPool &pool,
                             DescriptorSetConfiguration config)
        : mInstance(instance), mUboBuffer(uboBuffer) {
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
        const auto reserveResult = mUboBuffer.reserve(config.uboSize);
        require(!reserveResult.notEnoughSpace, "Could not allocate descriptor set - Buffer too small!");
        vk::DescriptorBufferInfo bufferInfo{
                uboBuffer.mBuffer,
                reserveResult.memoryIndex,
                config.uboSize
        };

        vk::WriteDescriptorSet descriptorWrite{
                mDescriptorSets[i],
                0,
                0,
                1,
                vk::DescriptorType::eUniformBuffer,
                nullptr,
                &bufferInfo,
                nullptr
        };

        mInstance.mDevice.updateDescriptorSets(descriptorWrite, nullptr);
    }
}

DescriptorSet::DescriptorSet(dn::vulkan::DescriptorSet &&other) noexcept
        : mInstance(other.mInstance),
          mUboBuffer(other.mUboBuffer),
          mDescriptorSets(std::exchange(other.mDescriptorSets, {})),
          mBufferIndices(std::exchange(other.mBufferIndices, {})) {
    log::d("Moving DescriptorSet");
}

DescriptorSet::~DescriptorSet() {
    log::d("Destroying DescriptorSet");

    // Descriptor Sets automatically cleared by Pool destruction?
}