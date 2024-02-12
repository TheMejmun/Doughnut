//
// Created by Sam on 2024-02-11.
//

#include "graphics/vulkan/buffer.h"
#include "io/logger.h"
#include "graphics/vulkan/memory.h"
#include "util/require.h"
#include "util/timer.h"

using namespace dn;
using namespace dn::vulkan;

constexpr uint32_t ALLOC_SIZE = 1024 * 1024 * 256;

Buffer::Buffer(dn::vulkan::Instance &instance, dn::vulkan::BufferConfiguration config)
        : mInstance(instance), mConfig(config) {
    log::d("Creating Buffer");

    mIsUsed.resize(ALLOC_SIZE);
    mIsUsedMutex = std::make_unique<std::mutex>();

    log::v("Maximum memory allocation count:", mInstance.mPhysicalDevice.getProperties().limits.maxMemoryAllocationCount);

    if (!mConfig.hostDirectAccessible) {
        vk::CommandPoolCreateInfo transferPoolInfo{
                vk::CommandPoolCreateFlags{vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
                mInstance.mQueueFamilyIndices.transferFamily.value()
        };
        mTransferCommandPool = mInstance.mDevice.createCommandPool(transferPoolInfo);

        vk::CommandBufferAllocateInfo transferBufferAllcateInfo{
                mTransferCommandPool,
                vk::CommandBufferLevel::ePrimary,
                1
        };
        mTransferCommandBuffer = mInstance.mDevice.allocateCommandBuffers(transferBufferAllcateInfo)[0];

        vk::FenceCreateInfo createInfo{};
        mTransferFence = mInstance.mDevice.createFence(createInfo);
    }

    vk::BufferUsageFlags usage;
    switch (mConfig.type) {
        case INDEX:
            usage = {vk::BufferUsageFlagBits::eIndexBuffer};
            break;
        case VERTEX:
            usage = {vk::BufferUsageFlagBits::eVertexBuffer};
            break;
        case UNIFORM:
            usage = {vk::BufferUsageFlagBits::eUniformBuffer};
            break;
    }
    if (!config.hostDirectAccessible) {
        usage |= vk::BufferUsageFlagBits::eTransferDst;
    }

    std::array<uint32_t, 2> queueFamilyIndices{
            mInstance.mQueueFamilyIndices.graphicsFamily.value(),
            mInstance.mQueueFamilyIndices.transferFamily.value()
    };
    vk::BufferCreateInfo bufferInfo{
            {},
            ALLOC_SIZE,
            usage,
            mInstance.mQueueFamilyIndices.hasUniqueTransferQueue() ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
            mInstance.mQueueFamilyIndices.hasUniqueTransferQueue() ? 2u : 1u,
            queueFamilyIndices.data()
    };
    mBuffer = mInstance.mDevice.createBuffer(bufferInfo);

    // Malloc
    vk::MemoryRequirements memoryRequirements = mInstance.mDevice.getBufferMemoryRequirements(mBuffer);
    vk::MemoryPropertyFlags propertyFlags{};
    if (config.hostDirectAccessible) {
        propertyFlags = {vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};
    } else {
        propertyFlags = {vk::MemoryPropertyFlagBits::eDeviceLocal};
    }

    vk::MemoryAllocateInfo allocInfo{
            memoryRequirements.size,
            findMemoryType(mInstance.mPhysicalDevice, memoryRequirements.memoryTypeBits, propertyFlags)
    };
    mBufferMemory = mInstance.mDevice.allocateMemory(allocInfo);

    // offset % memRequirements.alignment == 0
    mInstance.mDevice.bindBufferMemory(mBuffer, mBufferMemory, 0);

    // Persistent mapping:
    if (mConfig.hostDirectAccessible) {
        mMappedBuffer = static_cast<uint8_t *>(mInstance.mDevice.mapMemory(mBufferMemory, 0, ALLOC_SIZE, {}));
    }
}

Buffer::Buffer(dn::vulkan::Buffer &&other) noexcept
        : mInstance(other.mInstance),
          mConfig(other.mConfig),
          mTransferCommandPool(std::exchange(other.mTransferCommandPool, nullptr)),
          mTransferCommandBuffer(std::exchange(other.mTransferCommandBuffer, nullptr)),
          mTransferFence(std::exchange(other.mTransferFence, nullptr)),
          mStagingBuffer(std::exchange(other.mStagingBuffer, nullptr)),
          mStagingBufferMemory(std::exchange(other.mStagingBufferMemory, nullptr)),
          mBuffer(std::exchange(other.mBuffer, nullptr)),
          mBufferMemory(std::exchange(other.mBufferMemory, nullptr)),
          mIsUsed(std::exchange(other.mIsUsed, {})),
          mIsUsedMutex(std::exchange(other.mIsUsedMutex, nullptr)) {
    log::d("Moving Buffer");
}

UploadResult Buffer::calculateMemoryIndex(const uint32_t size) {
    uint32_t insertIndex = 0;
    {
        std::lock_guard<std::mutex> guard{*mIsUsedMutex};

        uint32_t remainingSize = size;
        for (uint32_t i = 0; i < mIsUsed.size(); ++i) {
            if (mIsUsed[i]) {
                remainingSize = size;
                insertIndex = i + 1; // Shift open block to after this one
            } else {
                remainingSize -= 1;
                if (remainingSize == 0) {
                    break;
                }
            }
        }

        if (remainingSize != 0) {
            log::e("Data does not fit into this buffer!");
            return {true, 0};
        }

        for (uint32_t i = insertIndex; i < insertIndex + size; ++i) {
            mIsUsed[i] = true;
        }

        log::v("Data of size", size, "will be uploaded at buffer position", insertIndex);
    }
    return {false, insertIndex};
}

UploadResult Buffer::queueUpload(const uint32_t size, const uint8_t *data) {
    require(!mConfig.hostDirectAccessible, "Can not queue uploads to a host accessible buffer");

    trace_scope(("Upload Queueing of data size " + std::to_string(size)));

    const auto [notEnoughSpace, index] = calculateMemoryIndex(size);
    if (notEnoughSpace) {
        return {true, 0};
    }

    awaitUpload();
    freeStagingMemory();

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

    vk::MemoryRequirements memoryRequirements = mInstance.mDevice.getBufferMemoryRequirements(mBuffer);
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

    vk::CommandBufferBeginInfo beginInfo{
            {vk::CommandBufferUsageFlagBits::eOneTimeSubmit}
    };
    mTransferCommandBuffer.begin(beginInfo);

    // Upload vertices
    vk::BufferCopy copyRegion{
            0,
            index,
            size
    };
    mTransferCommandBuffer.copyBuffer(mStagingBuffer, mBuffer, 1, &copyRegion);

    mTransferCommandBuffer.end();

    // Submit

    vk::SubmitInfo submitInfo{
            0,
            nullptr,
            nullptr,
            1,
            &mTransferCommandBuffer,
            0,
            nullptr,
    };

    mInstance.mTransferQueue.submit(submitInfo, mTransferFence);

    return {false, index};
}

UploadResult Buffer::directUpload(const uint32_t size, const uint8_t *data) {
    require(mConfig.hostDirectAccessible, "Can only access host accessible buffers directly");

    const auto [notEnoughSpace, index] = calculateMemoryIndex(size);
    if (notEnoughSpace) {
        return {true, 0};
    }

    memcpy(mMappedBuffer + index, data, size);
}

bool Buffer::isCurrentlyUploading() {
    auto result = mInstance.mDevice.getFenceStatus(mTransferFence);
    switch (result) {
        case vk::Result::eSuccess:
            return false;
        case vk::Result::eNotReady:
            return true;
        default:
            throw std::runtime_error("Upload fence returned a bad status");
    }
}

void Buffer::freeStagingMemory() {
    if (mStagingBuffer != nullptr) { mInstance.mDevice.destroy(mStagingBuffer); }
    if (mStagingBufferMemory != nullptr) { mInstance.mDevice.free(mStagingBufferMemory); }
}

void Buffer::awaitUpload(){
    if(!mConfig.hostDirectAccessible){
        auto result = mInstance.mDevice.waitForFences(mTransferFence, true, 30'000'000); // nanoseconds
        require(result == vk::Result::eSuccess || result == vk::Result::eTimeout, "An error has occurred while waiting for an upload to finish");
    }
    // Else noop
}

Buffer::~Buffer() {
    log::d("Destroying Buffer");

    if (mTransferFence != nullptr) {
        awaitUpload();
        mInstance.mDevice.destroy(mTransferFence);
    }

    freeStagingMemory();

    if (mBuffer != nullptr) { mInstance.mDevice.destroy(mBuffer); }
    if (mBufferMemory != nullptr) { mInstance.mDevice.free(mBufferMemory); }

    if (mTransferCommandBuffer != nullptr) { mInstance.mDevice.free(mTransferCommandPool, mTransferCommandBuffer); }
    if (mTransferCommandPool != nullptr) { mInstance.mDevice.destroy(mTransferCommandPool); }
}