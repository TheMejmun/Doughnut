//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/descriptor_pool.h"
#include "util/require.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

DescriptorPool::DescriptorPool(dn::vulkan::Instance &instance, dn::vulkan::DescriptorPoolConfiguration config)
        : mInstance(instance) {
    log::d("Creating DescriptorPool");
    // Can have multiple pools, with multiple buffers each
    vk::DescriptorPoolSize poolSize{
            vk::DescriptorType::eUniformBuffer,
            config.size
    };

    vk::DescriptorPoolCreateInfo poolInfo{
            {}, // Investigate VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
            // Would mean that Descriptor sets could individually be freed to their pools
            // Would allow vkFreeDescriptorSets
            // Otherwise only vkAllocateDescriptorSets and vkResetDescriptorPool
            config.size,
            1,
            &poolSize
    };

    mDescriptorPool = mInstance.mDevice.createDescriptorPool(poolInfo);
}

DescriptorPool::DescriptorPool(dn::vulkan::DescriptorPool &&other) noexcept
        : mInstance(other.mInstance), mDescriptorPool(std::exchange(other.mDescriptorPool, nullptr)) {
    log::d("Moving DescriptorPool");
}

DescriptorPool::~DescriptorPool() {
    log::d("Destroying DescriptorPool");
    if (mDescriptorPool != nullptr) { mInstance.mDevice.destroy(mDescriptorPool); }
}