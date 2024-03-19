//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/fence.h"
#include "io/logger.h"
#include "util/timer.h"
#include "util/require.h"

using namespace dn;
using namespace dn::vulkan;

Fence::Fence(Context &context,
             FenceConfiguration config)
        : mContext(context) {
    log::d("Creating Fence");
    vk::FenceCreateInfo fenceInfo{
            config.startSignalled ?
            vk::FenceCreateFlags{vk::FenceCreateFlagBits::eSignaled} :
            vk::FenceCreateFlags{}
    };
    mFence = mContext.mDevice.createFence(fenceInfo);
}

Fence::Fence(Fence &&other) noexcept
        : mContext(other.mContext), mFence(std::exchange(other.mFence, nullptr)) {
    log::d("Moving Fence");
}

double Fence::await() const {
    auto beforeFence = now();
    auto result = mContext.mDevice.waitForFences(mFence, vk::True, std::numeric_limits<uint64_t>::max());
    require(result == vk::Result::eSuccess || result == vk::Result::eTimeout, "An error has occurred while waiting for a fence");
    auto afterFence = now();

    return duration(beforeFence, afterFence);
}

void Fence::resetFence() const {
    mContext.mDevice.resetFences(mFence);
}

bool Fence::isWaiting() const {
    auto result = mContext.mDevice.getFenceStatus(mFence);
    switch (result) {
        case vk::Result::eSuccess:
            return false;
        case vk::Result::eNotReady:
            return true;
        default:
            throw std::runtime_error("Upload fence returned a bad status");
    }
}

Fence::~Fence() {
    log::d("Destroying Fence");
    await();
    if (mFence != nullptr) { mContext.mDevice.destroy(mFence); }
}