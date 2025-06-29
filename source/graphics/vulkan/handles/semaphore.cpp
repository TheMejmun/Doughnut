//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/handles/semaphore.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

Semaphore::Semaphore(Context &context, const SemaphoreConfiguration &config)
        : Handle<vk::Semaphore, SemaphoreConfiguration>(context, config) {
    vk::SemaphoreCreateInfo semaphoreInfo{};
    mVulkan = mContext.mDevice.createSemaphore(semaphoreInfo);

    this->registerDebug();
}

Semaphore::~Semaphore() {
    if (mVulkan != nullptr) { mContext.mDevice.destroy(mVulkan); }
}