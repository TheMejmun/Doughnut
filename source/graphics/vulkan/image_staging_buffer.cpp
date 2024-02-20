//
// Created by Sam on 2024-02-19.
//

#include "graphics/vulkan/image_staging_buffer.h"
#include "io/logger.h"
#include "util/require.h"
#include "util/timer.h"
#include "graphics/vulkan/memory.h"

using namespace dn;
using namespace dn::vulkan;

ImageStagingBuffer::ImageStagingBuffer(dn::vulkan::Instance &instance, dn::vulkan::ImageStagingBufferConfiguration config)
        : mInstance(instance),
          mConfig(config) {
    log::d("Creating ImageStagingBuffer");

    mCommandPool.emplace(mInstance,
                         CommandPoolConfiguration{*mInstance.mQueueFamilyIndices.transferFamily});
    mCommandBuffer.emplace(mInstance,
                           *mCommandPool);
    mFence.emplace(mInstance,
                   FenceConfiguration{true});
}

ImageStagingBuffer::ImageStagingBuffer(dn::vulkan::ImageStagingBuffer &&other) noexcept
        : mInstance(other.mInstance),
          mConfig(other.mConfig),
          mCommandPool(std::exchange(other.mCommandPool, nullptr)),
          mCommandBuffer(std::exchange(other.mCommandBuffer, nullptr)),
          mFence(std::exchange(other.mFence, nullptr)),
          mStagingBuffer(std::exchange(other.mStagingBuffer, nullptr)),
          mStagingBufferMemory(std::exchange(other.mStagingBufferMemory, nullptr)) {
    log::d("Moving ImageStagingBuffer");
}

void ImageStagingBuffer::upload(const Texture& texture, vk::Buffer target) {
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
            texture.size(),
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
    void *mappedMemory = mInstance.mDevice.mapMemory(mStagingBufferMemory, 0, texture.size(), {});
    memcpy(mappedMemory, texture.mData, texture.size());
    mInstance.mDevice.unmapMemory(mStagingBufferMemory);

    // To final buffer
    vk::BufferCopy copyRegion{
            0,
            0,
            texture.size()
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

bool ImageStagingBuffer::isCurrentlyUploading() {
    return mFence->isWaiting();
}

void ImageStagingBuffer::freeStagingMemory() {
    if (mStagingBuffer != nullptr) { mInstance.mDevice.destroy(mStagingBuffer); }
    if (mStagingBufferMemory != nullptr) { mInstance.mDevice.free(mStagingBufferMemory); }
}

void ImageStagingBuffer::awaitUpload() {
    mFence->await();
}

ImageStagingBuffer::~ImageStagingBuffer() {
    log::d("Destroying ImageStagingBuffer");

    mFence.clear();
    freeStagingMemory();

    mCommandBuffer.clear();
    mCommandPool.clear();
}