//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/command_pool.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

CommandPool::CommandPool(Context &context, const CommandPoolConfiguration &config)
        : Handle<vk::CommandPool, CommandPoolConfiguration>(context, config){

    // Use VK_COMMAND_POOL_CREATE_TRANSIENT_BIT if buffer is very short-lived
    vk::CommandPoolCreateInfo poolInfo{
            {vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
            config.queueFamilyIndex
    };

    mVulkan = mContext.mDevice.createCommandPool(poolInfo);
}

CommandPool::~CommandPool() {
    if (mVulkan != nullptr) { mContext.mDevice.destroy(mVulkan); }
}