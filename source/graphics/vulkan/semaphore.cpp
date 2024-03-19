//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/semaphore.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

Semaphore::Semaphore(Context &context)
        : mContext(context) {
    log::d("Creating Semaphore");
    vk::SemaphoreCreateInfo semaphoreInfo{};
        mSemaphore = mContext.mDevice.createSemaphore(semaphoreInfo);
}

Semaphore::Semaphore(dn::vulkan::Semaphore &&other) noexcept
        : mContext(other.mContext), mSemaphore(std::exchange(other.mSemaphore, nullptr)) {
    log::d("Moving Semaphore");
}

Semaphore::~Semaphore() {
    log::d("Destroying Semaphore");
    if (mSemaphore != nullptr) { mContext.mDevice.destroy(mSemaphore); }
}