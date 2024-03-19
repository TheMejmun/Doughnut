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

StagingBuffer::StagingBuffer(Context &context, StagingBufferConfiguration config)
        : mContext(context),
          mConfig(config),
          mCommandPool(mContext, CommandPoolConfiguration{*mContext.mQueueFamilyIndices.transferFamily}),
          mCommandBuffer(mContext, mCommandPool),
          mFence(mContext, FenceConfiguration{true}) {
    log::d("Creating StagingBuffer");
}

void StagingBuffer::upload(const uint32_t size, const void *data, const vk::Buffer target, const uint32_t at) {
    trace_scope("Upload Queueing");

    awaitUpload();
    mFence.resetFence();
    mCommandBuffer.startRecording();

    // Transfer Buffer Creation
    std::array<uint32_t, 2> queueFamilyIndices{
            mContext.mQueueFamilyIndices.graphicsFamily.value(),
            mContext.mQueueFamilyIndices.transferFamily.value()
    };
    vk::BufferCreateInfo bufferInfo{
            {},
            size,
            {vk::BufferUsageFlagBits::eTransferSrc},
            mContext.mQueueFamilyIndices.hasUniqueTransferQueue() ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
            mContext.mQueueFamilyIndices.hasUniqueTransferQueue() ? 2u : 1u,
            queueFamilyIndices.data()
    };
    mStagingBuffer = mContext.mDevice.createBuffer(bufferInfo);

    vk::MemoryRequirements memoryRequirements = mContext.mDevice.getBufferMemoryRequirements(target);
    vk::MemoryAllocateInfo allocInfo{
            memoryRequirements.size,
            findMemoryType(mContext.mPhysicalDevice, memoryRequirements.memoryTypeBits,
                           {vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent})
    };
    mStagingBufferMemory = mContext.mDevice.allocateMemory(allocInfo);

    mContext.mDevice.bindBufferMemory(mStagingBuffer, mStagingBufferMemory, 0);

    // Upload To Transfer Buffer
    void *mappedMemory = mContext.mDevice.mapMemory(mStagingBufferMemory, 0, size, {});
    memcpy(mappedMemory, data, size);
    mContext.mDevice.unmapMemory(mStagingBufferMemory);

    // To final buffer
    vk::BufferCopy copyRegion{
            0,
            at,
            size
    };
    mCommandBuffer.mCommandBuffer.copyBuffer(mStagingBuffer, target, 1, &copyRegion);

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
    mContext.mTransferQueue.submit(submitInfo, *mFence);
}

bool StagingBuffer::isCurrentlyUploading() {
    return mFence.isWaiting();
}

void StagingBuffer::freeStagingMemory() {
    if (mStagingBuffer != nullptr) { mContext.mDevice.destroy(mStagingBuffer); }
    if (mStagingBufferMemory != nullptr) { mContext.mDevice.free(mStagingBufferMemory); }
}

void StagingBuffer::awaitUpload() {
    mFence.await();
}

StagingBuffer::~StagingBuffer() {
    log::d("Destroying StagingBuffer");

    mFence.await();
    freeStagingMemory();
}