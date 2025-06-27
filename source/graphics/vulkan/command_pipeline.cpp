//
// Created by Sam on 2024-04-04.
//

#include "graphics/vulkan/command_pipeline.h"
#include "io/logger.h"
#include "util/require.h"

using namespace dn;
using namespace dn::vulkan;

uint32_t getQueueFamilyIndex(Context &context, Queue queue) {
    switch (queue) {
        case GRAPHICS:
            require(context.mQueueFamilyIndices.graphicsFamily.has_value(), "Requested queue family was not found");
            return *context.mQueueFamilyIndices.graphicsFamily;
        case PRESENT:
            require(context.mQueueFamilyIndices.presentFamily.has_value(), "Requested queue family was not found");
            return *context.mQueueFamilyIndices.presentFamily;
        case TRANSFER:
            require(context.mQueueFamilyIndices.transferFamily.has_value(), "Requested queue family was not found");
            return *context.mQueueFamilyIndices.transferFamily;
    }
}

CommandPipeline::CommandPipeline(Context &context, const CommandPipelineConfiguration &config)
        : mContext(context),
          mConfig(config),
          mCommandPool(context, CommandPoolConfiguration{getQueueFamilyIndex(mContext, mConfig.queue)}),
          mCommandBuffers() {

    mCommandBuffers.reserve(mConfig.bufferCount);
    for (uint32_t i = 0; i < mConfig.bufferCount; ++i) {
        mCommandBuffers.emplace_back(
                mContext,
                mCommandPool,
                CommandBufferConfiguration{}
        );
    }

    log::v("Created CommandPipeline");
}

CommandBuffer *CommandPipeline::operator->() {
    return &mCommandBuffers[mCurrentBuffer];
}

const CommandBuffer *CommandPipeline::operator->() const {
    return &mCommandBuffers[mCurrentBuffer];
}

CommandBuffer &CommandPipeline::operator*() {
    return mCommandBuffers[mCurrentBuffer];
}

const CommandBuffer &CommandPipeline::operator*() const {
    return mCommandBuffers[mCurrentBuffer];
}

void CommandPipeline::nextBuffer() {
    mCurrentBuffer = (mCurrentBuffer + 1) % mConfig.bufferCount;
}