//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/fence.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

Fence::Fence(Instance &instance,
             FenceConfiguration config)
        : mInstance(instance) {
    log::d("Creating Fence");
    vk::FenceCreateInfo fenceInfo{
            config.startSignalled ?
            vk::FenceCreateFlags{vk::FenceCreateFlagBits::eSignaled} :
            vk::FenceCreateFlags{}
    };
    mFence = mInstance.mDevice.createFence(fenceInfo);
}

Fence::Fence(dn::vulkan::Fence &&other) noexcept
        : mInstance(other.mInstance), mFence(std::exchange(other.mFence, nullptr)) {
    log::d("Moving Fence");
}

Fence::~Fence() {
    log::d("Destroying Fence");
    if (mFence != nullptr) { mInstance.mDevice.destroy(mFence); }
}