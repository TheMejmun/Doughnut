//
// Created by Sam on 2024-02-18.
//

#include "graphics/vulkan/staging_buffer.h"
#include "io/logger.h"
#include "util/require.h"
#include "util/timer.h"
#include "graphics/vulkan/memory.h"

using namespace dn;
using namespace dn::vulkan;

StagingBuffer::StagingBuffer(dn::vulkan::Instance &instance, dn::vulkan::StagingBufferConfiguration config)
        : mInstance(instance),
          mConfig(config) {
    log::d("Creating StagingBuffer");

    mCommandPool.emplace(mInstance,
                         CommandPoolConfiguration{*mInstance.mQueueFamilyIndices.transferFamily});
    mCommandBuffer.emplace(mInstance,
                           *mCommandPool);
    mFence.emplace(mInstance,
                   FenceConfiguration{true});
}

StagingBuffer::StagingBuffer(dn::vulkan::StagingBuffer &&other) noexcept
        : mInstance(other.mInstance),
          mConfig(other.mConfig),
          mCommandPool(std::exchange(other.mCommandPool, nullptr)),
          mCommandBuffer(std::exchange(other.mCommandBuffer, nullptr)),
          mFence(std::exchange(other.mFence, nullptr)),
          mStagingBuffer(std::exchange(other.mStagingBuffer, nullptr)),
          mStagingBufferMemory(std::exchange(other.mStagingBufferMemory, nullptr)) {
    log::d("Moving StagingBuffer");
}

void StagingBuffer::upload(const uint32_t size, const void *data, const vk::Buffer target, const uint32_t at) {
    trace_scope("Upload Queueing");

    awaitUpload();
    mFence->resetFence();
    mCommandBuffer->startRecording();

    // Transfer Buffer Creation
    std::array<uint32_t, 2> queueFamilyIndices{
            mInstance.mQueueFamilyIndices.graphicsFamily.value(),
            mInstance.mQueueFamilyIndices.transferFamily.value()
    };
    vk::BufferCreateInfo bufferInfo{
            {},
            size,
            {vk::BufferUsageFlagBits::eTransferSrc},
            mInstance.mQueueFamilyIndices.hasUniqueTransferQueue() ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
            mInstance.mQueueFamilyIndices.hasUniqueTransferQueue() ? 2u : 1u,
            queueFamilyIndices.data()
    };
    mStagingBuffer = mInstance.mDevice.createBuffer(bufferInfo);

    vk::MemoryRequirements memoryRequirements = mInstance.mDevice.getBufferMemoryRequirements(target);
    vk::MemoryAllocateInfo allocInfo{
            memoryRequirements.size,
            findMemoryType(mInstance.mPhysicalDevice, memoryRequirements.memoryTypeBits,
                           {vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent})
    };
    mStagingBufferMemory = mInstance.mDevice.allocateMemory(allocInfo);

    mInstance.mDevice.bindBufferMemory(mStagingBuffer, mStagingBufferMemory, 0);

    // Upload To Transfer Buffer
    void *mappedMemory = mInstance.mDevice.mapMemory(mStagingBufferMemory, 0, size, {});
    memcpy(mappedMemory, data, size);
    mInstance.mDevice.unmapMemory(mStagingBufferMemory);

    // To final buffer
    vk::BufferCopy copyRegion{
            0,
            at,
            size
    };
    mCommandBuffer->mCommandBuffer.copyBuffer(mStagingBuffer, target, 1, &copyRegion);

    mCommandBuffer->endRecording();

    vk::SubmitInfo submitInfo{
            0,
            nullptr,
            nullptr,
            1,
            &mCommandBuffer->mCommandBuffer,
            0,
            nullptr,
    };
    mInstance.mTransferQueue.submit(submitInfo, mFence->mFence);
}

bool StagingBuffer::isCurrentlyUploading() {
    return mFence->isWaiting();
}

void StagingBuffer::freeStagingMemory() {
    if (mStagingBuffer != nullptr) { mInstance.mDevice.destroy(mStagingBuffer); }
    if (mStagingBufferMemory != nullptr) { mInstance.mDevice.free(mStagingBufferMemory); }
}

void StagingBuffer::awaitUpload() {
    mFence->await();
}

StagingBuffer::~StagingBuffer() {
    log::d("Destroying StagingBuffer");

    mFence.clear();
    freeStagingMemory();

    mCommandBuffer.clear();
    mCommandPool.clear();
}