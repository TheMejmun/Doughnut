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

Buffer::Buffer(dn::vulkan::Context &context, dn::vulkan::BufferConfiguration config)
        : mContext(context), mConfig(config) {
    log::d("Creating Buffer");

    mIsUsed.resize(ALLOC_SIZE);
    mIsUsedMutex = std::make_unique<std::mutex>();

    log::v("Maximum memory allocation count:", mContext.mPhysicalDevice.getProperties().limits.maxMemoryAllocationCount);

    if (!mConfig.hostDirectAccessible) {
        mStagingBuffer.emplace(
                mContext,
                StagingBufferConfiguration{}
        );
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
            mContext.mQueueFamilyIndices.graphicsFamily.value(),
            mContext.mQueueFamilyIndices.transferFamily.value()
    };
    vk::BufferCreateInfo bufferInfo{
            {},
            ALLOC_SIZE,
            usage,
            mContext.mQueueFamilyIndices.hasUniqueTransferQueue() ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
            mContext.mQueueFamilyIndices.hasUniqueTransferQueue() ? 2u : 1u,
            queueFamilyIndices.data()
    };
    mBuffer = mContext.mDevice.createBuffer(bufferInfo);

    // Malloc
    vk::MemoryRequirements memoryRequirements = mContext.mDevice.getBufferMemoryRequirements(mBuffer);
    vk::MemoryPropertyFlags propertyFlags{};
    if (config.hostDirectAccessible) {
        propertyFlags = {vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};
    } else {
        propertyFlags = {vk::MemoryPropertyFlagBits::eDeviceLocal};
    }

    vk::MemoryAllocateInfo allocInfo{
            memoryRequirements.size,
            findMemoryType(mContext.mPhysicalDevice, memoryRequirements.memoryTypeBits, propertyFlags)
    };
    mBufferMemory = mContext.mDevice.allocateMemory(allocInfo);

    // offset % memRequirements.alignment == 0
    mContext.mDevice.bindBufferMemory(mBuffer, mBufferMemory, 0);

    // Persistent mapping:
    if (mConfig.hostDirectAccessible) {
        mMappedBuffer = static_cast<uint8_t *>(mContext.mDevice.mapMemory(mBufferMemory, 0, ALLOC_SIZE, {}));
    }
}

Buffer::Buffer(dn::vulkan::Buffer &&other) noexcept
        : mContext(other.mContext),
          mConfig(other.mConfig),
          mStagingBuffer(std::exchange(other.mStagingBuffer, nullptr)),
          mBuffer(std::exchange(other.mBuffer, nullptr)),
          mBufferMemory(std::exchange(other.mBufferMemory, nullptr)),
          mIsUsed(std::exchange(other.mIsUsed, {})),
          mIsUsedMutex(std::exchange(other.mIsUsedMutex, nullptr)) {
    log::d("Moving Buffer");
}

UploadResult Buffer::calculateMemoryIndex(const uint32_t size) {
    // TODO memory alignment requirements
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
            return {true, 0, 0};
        }

        for (uint32_t i = insertIndex; i < insertIndex + size; ++i) {
            mIsUsed[i] = true;
        }

        log::v("Data of size", size, "will be uploaded at buffer position", insertIndex);
    }
    return {false, insertIndex, size};
}

UploadResult Buffer::reserve(const uint32_t size) {
    return calculateMemoryIndex(size);
}

UploadResult Buffer::queueUpload(const uint32_t size, const uint8_t *data) {
    dnAssert(!mConfig.hostDirectAccessible, "Can not queue uploads to a host accessible buffer");

    const auto location = calculateMemoryIndex(size);
    if (!location.notEnoughSpace) {
        queueUpload(size, data, location.position.memoryIndex);
    }

    return location;
}

void Buffer::queueUpload(const uint32_t size, const uint8_t *data, const uint32_t at) {
    dnAssert(!mConfig.hostDirectAccessible, "Can not queue uploads to a host accessible buffer");

    awaitUpload();

    mStagingBuffer->upload(
            size,
            data,
            mBuffer,
            at
    );
}

UploadResult Buffer::directUpload(const uint32_t size, const uint8_t *data) {
    dnAssert(mConfig.hostDirectAccessible, "Can only access host accessible buffers directly");

    const auto location = calculateMemoryIndex(size);
    if (!location.notEnoughSpace) {
        directUpload(size, data, location.position.memoryIndex);
    }

    return location;
}

void Buffer::directUpload(const uint32_t size, const uint8_t *data, const uint32_t at) {
    dnAssert(mConfig.hostDirectAccessible, "Can only access host accessible buffers directly");

    trace_scope("Direct Upload");

    memcpy(mMappedBuffer + at, data, size);
}

void Buffer::clear(uint32_t at, uint32_t byteSize) {
    std::lock_guard<std::mutex> guard{*mIsUsedMutex};

    for (uint32_t i = at; i < (at + byteSize); ++i) {
        mIsUsed[i] = false;
    }
}

bool Buffer::isCurrentlyUploading() {
    return !mConfig.hostDirectAccessible && mStagingBuffer->isCurrentlyUploading();
}

void Buffer::freeStagingMemory() {
    if (mStagingBuffer.has_value())mStagingBuffer->freeStagingMemory();
}

void Buffer::awaitUpload() {
    if (mStagingBuffer.has_value())mStagingBuffer->awaitUpload();
}

Buffer::~Buffer() {
    log::d("Destroying Buffer");

    mStagingBuffer.reset();

    if (mBuffer != nullptr) { mContext.mDevice.destroy(mBuffer); }
    if (mBufferMemory != nullptr) { mContext.mDevice.free(mBufferMemory); }
}