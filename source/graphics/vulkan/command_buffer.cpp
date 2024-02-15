//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/command_buffer.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

CommandBuffer::CommandBuffer(Instance &instance,
                             CommandPool &pool)
        : mInstance(instance) {
    log::d("Creating CommandBuffer");

    // TODO multiple? so we won't have to re-record all of them
    vk::CommandBufferAllocateInfo allocInfo{
            pool.mCommandPool,
            vk::CommandBufferLevel::ePrimary,
            1
    };

    mCommandBuffer = mInstance.mDevice.allocateCommandBuffers(allocInfo)[0];
}

CommandBuffer::CommandBuffer(dn::vulkan::CommandBuffer &&other) noexcept
        : mInstance(other.mInstance),
          mCommandBuffer(std::exchange(other.mCommandBuffer, nullptr)) {
    log::d("Moving CommandBuffer");
}

void CommandBuffer::reset() const {
    mCommandBuffer.reset();
}

void CommandBuffer::startRecording() const {
    reset();

    vk::CommandBufferBeginInfo beginInfo{
            {},
            nullptr
    };

    mCommandBuffer.begin(beginInfo);
}

void CommandBuffer::endRecording() const {
    mCommandBuffer.end();
}

CommandBuffer::~CommandBuffer() {
    log::d("Destroying CommandBuffer");

    // Not necessary. Pool destruction will handle this
}