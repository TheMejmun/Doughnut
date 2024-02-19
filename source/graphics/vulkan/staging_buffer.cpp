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
          mConfig(config),
          mCommandPool(mInstance, {*mInstance.mQueueFamilyIndices.transferFamily}),
          mCommandBuffer(mInstance, mCommandPool),
          mFence(mInstance, {true}) {
    log::d("Creating StagingBuffer");
}

void StagingBuffer::startCommandBuffer() {
    awaitUpload();
    mCommandBuffer.startRecording();
}

void StagingBuffer::submit() {
    mCommandBuffer.endRecording();

    vk::SubmitInfo submitInfo{
            0,
            nullptr,
            nullptr,
            1,
            &mCommandBuffer.mCommandBuffer,
            0,
            nullptr,
    };
    mInstance.mTransferQueue.submit(submitInfo, mFence.mFence);
}

void StagingBuffer::uploadCommand(uint32_t size, void *data, vk::Buffer target, uint32_t at) {
    trace_scope("Upload Queueing");

    if (!mCommandBuffer.mIsRecording) {
        startCommandBuffer();
    }

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
    mCommandBuffer.mCommandBuffer.copyBuffer(mStagingBuffer, target, 1, &copyRegion);
}

bool StagingBuffer::isCurrentlyUploading() {
    mFence.isWaiting();
}

void StagingBuffer::freeStagingMemory() {
    if (mStagingBuffer != nullptr) { mInstance.mDevice.destroy(mStagingBuffer); }
    if (mStagingBufferMemory != nullptr) { mInstance.mDevice.free(mStagingBufferMemory); }
}

void StagingBuffer::awaitUpload() {
    mFence.await();
}

StagingBuffer::~StagingBuffer() {
    log::d("Destroying StagingBuffer");

    if (mTransferFence != nullptr) {
        awaitUpload();
        mInstance.mDevice.destroy(mTransferFence);
    }

    if (mTransferCommandBuffer != nullptr) { mInstance.mDevice.free(mTransferCommandPool, mTransferCommandBuffer); }
    if (mTransferCommandPool != nullptr) { mInstance.mDevice.destroy(mTransferCommandPool); }
}