//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/fence.h"
#include "io/logger.h"
#include "util/timer.h"
#include "util/require.h"

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

double Fence::await() const {
    auto beforeFence = now();
    auto result = mInstance.mDevice.waitForFences(mFence, vk::True, std::numeric_limits<uint64_t>::max());
    require(result == vk::Result::eSuccess || result == vk::Result::eTimeout, "An error has occurred while waiting for a fence");
    auto afterFence = now();

    return duration(beforeFence, afterFence);
}

void Fence::resetFence() const {
    mInstance.mDevice.resetFences(mFence);
}

Fence::~Fence() {
    log::d("Destroying Fence");
    if (mFence != nullptr) { mInstance.mDevice.destroy(mFence); }
}