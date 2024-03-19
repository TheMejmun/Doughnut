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
        : Handle<vk::Fence, FenceConfiguration>(context, config) {

    vk::FenceCreateInfo fenceInfo{
            mConfig.startSignalled ?
            vk::FenceCreateFlags{vk::FenceCreateFlagBits::eSignaled} :
            vk::FenceCreateFlags{}
    };
    mVulkan = mContext.mDevice.createFence(fenceInfo);
}

double Fence::await() const {
    auto beforeFence = now();
    auto result = mContext.mDevice.waitForFences(mVulkan, vk::True, std::numeric_limits<uint64_t>::max());
    require(result == vk::Result::eSuccess || result == vk::Result::eTimeout, "An error has occurred while waiting for a fence");
    auto afterFence = now();

    return duration(beforeFence, afterFence);
}

void Fence::resetFence() const {
    mContext.mDevice.resetFences(mVulkan);
}

bool Fence::isWaiting() const {
    auto result = mContext.mDevice.getFenceStatus(mVulkan);
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
    await();
    if (mVulkan != nullptr) { mContext.mDevice.destroy(mVulkan); }
}