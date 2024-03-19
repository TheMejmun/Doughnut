//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/descriptor_pool.h"
#include "util/require.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

DescriptorPool::DescriptorPool(Context &context, const DescriptorPoolConfiguration& config)
        : mContext(context) {
    log::d("Creating DescriptorPool");
    // Can have multiple pools, with multiple buffers each
    std::vector<vk::DescriptorPoolSize> poolSizes{};

    for (const auto &size: config.sizes) {
        vk::DescriptorType type;
        switch (size.type) {
            case UNIFORM_BUFFER:
                type = vk::DescriptorType::eUniformBuffer;
                break;
            case SAMPLER:
                type = vk::DescriptorType::eCombinedImageSampler;
                break;
        }
        poolSizes.emplace_back(type,
                               size.count
        );
    }

    vk::DescriptorPoolCreateFlags flags;
    if (config.freeable) {
        flags |= vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    }

    vk::DescriptorPoolCreateInfo poolInfo{
            flags, // Investigate VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
            // Would mean that Descriptor sets could individually be freed to their pools
            // Would allow vkFreeDescriptorSets
            // Otherwise only vkAllocateDescriptorSets and vkResetDescriptorPool
            config.maxFramesInFlight, // TODO
            static_cast<uint32_t>(poolSizes.size()),
            poolSizes.data()
    };

    mDescriptorPool = mContext.mDevice.createDescriptorPool(poolInfo);
}

DescriptorPool::DescriptorPool(dn::vulkan::DescriptorPool &&other) noexcept
        : mContext(other.mContext), mDescriptorPool(std::exchange(other.mDescriptorPool, nullptr)) {
    log::d("Moving DescriptorPool");
}

DescriptorPool::~DescriptorPool() {
    log::d("Destroying DescriptorPool");
    if (mDescriptorPool != nullptr) { mContext.mDevice.destroy(mDescriptorPool); }
}