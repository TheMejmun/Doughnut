//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/command_buffer.h"
#include "io/logger.h"
#include "util/require.h"

using namespace dn;
using namespace dn::vulkan;

CommandBuffer::CommandBuffer(Context &context,
                             CommandPool &pool)
        : mContext(context) {
    log::d("Creating CommandBuffer");

    // TODO multiple? so we won't have to re-record all of them
    vk::CommandBufferAllocateInfo allocInfo{
            pool.mCommandPool,
            vk::CommandBufferLevel::ePrimary,
            1
    };

    mCommandBuffer = mContext.mDevice.allocateCommandBuffers(allocInfo)[0];
}

CommandBuffer::CommandBuffer(dn::vulkan::CommandBuffer &&other) noexcept
        : mContext(other.mContext),
          mCommandBuffer(std::exchange(other.mCommandBuffer, nullptr)) {
    log::d("Moving CommandBuffer");
}

void CommandBuffer::reset() const {
    mCommandBuffer.reset();
}

void CommandBuffer::startRecording() {
    dnAssert(!mIsRecording, "Can not start recording a command buffer that is already recording");

    reset();

    vk::CommandBufferBeginInfo beginInfo{
            {},
            nullptr
    };

    mCommandBuffer.begin(beginInfo);

    mIsRecording = true;
}

void CommandBuffer::endRecording() {
    dnAssert(mIsRecording, "Can not end recording a command buffer that is not recording");

    mCommandBuffer.end();
    mIsRecording = false;
}

CommandBuffer::~CommandBuffer() {
    log::d("Destroying CommandBuffer");

    // Not necessary. Pool destruction will handle this
}