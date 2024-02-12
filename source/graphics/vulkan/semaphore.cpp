//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/semaphore.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

Semaphore::Semaphore(dn::vulkan::Instance &instance)
        : mInstance(instance) {
    log::d("Creating Semaphore");
    vk::SemaphoreCreateInfo semaphoreInfo{};
    mSemaphore = mInstance.mDevice.createSemaphore(semaphoreInfo);
}

Semaphore::Semaphore(dn::vulkan::Semaphore &&other) noexcept
        : mInstance(other.mInstance), mSemaphore(std::exchange(other.mSemaphore, nullptr)) {
    log::d("Moving Semaphore");
}

Semaphore::~Semaphore() {
    log::d("Destroying Semaphore");
    if (mSemaphore != nullptr) { mInstance.mDevice.destroy(mSemaphore); }
}