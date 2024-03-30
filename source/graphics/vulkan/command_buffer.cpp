//
// Created by Sam on 2024-02-12.
//

#include "graphics/vulkan/command_buffer.h"
#include "io/logger.h"
#include "util/require.h"

using namespace dn;
using namespace dn::vulkan;

CommandBuffer::CommandBuffer(Context &context,
                             CommandPool &pool,
                             const CommandBufferConfiguration& config)
        : Handle<vk::CommandBuffer, CommandBufferConfiguration>(context, config) {
    log::d("Creating CommandBuffer");

    // TODO multiple? so we won't have to re-record all of them
    vk::CommandBufferAllocateInfo allocInfo{
            *pool,
            vk::CommandBufferLevel::ePrimary,
            1
    };

    mVulkan = mContext.mDevice.allocateCommandBuffers(allocInfo)[0];
}

void CommandBuffer::reset() const {
    mVulkan.reset();
}

void CommandBuffer::startRecording() {
    require_d(!mIsRecording, "Can not start recording a command buffer that is already recording");

    reset();

    vk::CommandBufferBeginInfo beginInfo{
            {},
            nullptr
    };

    mVulkan.begin(beginInfo);

    mIsRecording = true;
}

void CommandBuffer::endRecording() {
    require_d(mIsRecording, "Can not end recording a command buffer that is not recording");

    mVulkan.end();
    mIsRecording = false;
}