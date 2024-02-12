//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/command_pool.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

CommandPool::CommandPool(Instance &instance)
        : mInstance(instance) {
    log::d("Creating CommandPool");

    // Use VK_COMMAND_POOL_CREATE_TRANSIENT_BIT if buffer is very short-lived
    vk::CommandPoolCreateInfo poolInfo{
            {vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
            *mInstance.mQueueFamilyIndices.graphicsFamily
    };

    mCommandPool = mInstance.mDevice.createCommandPool(poolInfo);
}

CommandPool::CommandPool(dn::vulkan::CommandPool &&other) noexcept
        : mInstance(other.mInstance),
          mCommandPool(std::exchange(other.mCommandPool, nullptr)) {
    log::d("Moving CommandPool");
}

CommandPool::~CommandPool() {
    log::d("Destroying CommandPool");
    if (mCommandPool != nullptr) { mInstance.mDevice.destroy(mCommandPool); }
}