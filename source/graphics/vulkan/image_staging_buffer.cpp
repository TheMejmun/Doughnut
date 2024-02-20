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
                         CommandPoolConfiguration{*mInstance.mQueueFamilyIndices.graphicsFamily});
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

void ImageStagingBuffer::upload(const Texture &texture, vk::Image target) {
    trace_scope("Upload Queueing");

    awaitUpload();
    mFence->resetFence();
    mCommandBuffer->startRecording();

    // -------------------- STAGING --------------------

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

    vk::MemoryRequirements memoryRequirements = mInstance.mDevice.getBufferMemoryRequirements(mStagingBuffer);
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

    // -------------------- TRANSITION --------------------

    vk::ImageMemoryBarrier barrier{
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eTransferWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal,
            vk::QueueFamilyIgnored,
            vk::QueueFamilyIgnored,
            target,
            vk::ImageSubresourceRange{
                    {vk::ImageAspectFlagBits::eColor},
                    0,
                    1,
                    0,
                    1
            }
    };

    // https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap7.html#synchronization-access-types-supported
    mCommandBuffer->mCommandBuffer.pipelineBarrier(
            {vk::PipelineStageFlagBits::eTopOfPipe},
            {vk::PipelineStageFlagBits::eTransfer},
            {}, // VK_DEPENDENCY_BY_REGION_BIT https://stackoverflow.com/questions/65471677/the-meaning-and-implications-of-vk-dependency-by-region-bit
            0, nullptr,
            0, nullptr,
            1, &barrier
    );

    // -------------------- COPY --------------------

    vk::BufferImageCopy region{
            0,
            0,
            0,
            vk::ImageSubresourceLayers{
                    {vk::ImageAspectFlagBits::eColor},
                    0,
                    0,
                    1,
            },
            {0, 0, 0},
            {static_cast<uint32_t>(texture.mWidth),
             static_cast<uint32_t>(texture.mHeight),
             1}
    };

    mCommandBuffer->mCommandBuffer.copyBufferToImage(
            mStagingBuffer,
            target,
            vk::ImageLayout::eTransferDstOptimal,
            1,
            &region);

    // -------------------- TRANSITION --------------------


    vk::ImageMemoryBarrier barrier2{
            vk::AccessFlagBits::eTransferWrite,
            vk::AccessFlagBits::eShaderRead,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::QueueFamilyIgnored,
            vk::QueueFamilyIgnored,
            target,
            vk::ImageSubresourceRange{
                    {vk::ImageAspectFlagBits::eColor},
                    0,
                    1,
                    0,
                    1
            }
    };

    // https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap7.html#synchronization-access-types-supported
    mCommandBuffer->mCommandBuffer.pipelineBarrier(
            {vk::PipelineStageFlagBits::eTransfer},
            {vk::PipelineStageFlagBits::eFragmentShader},
            {}, // VK_DEPENDENCY_BY_REGION_BIT https://stackoverflow.com/questions/65471677/the-meaning-and-implications-of-vk-dependency-by-region-bit
            0, nullptr,
            0, nullptr,
            1, &barrier2
    );

//    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//    copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    // -------------------- TODO --------------------

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
    mInstance.mGraphicsQueue.submit(submitInfo, mFence->mFence);
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