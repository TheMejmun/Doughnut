//
// Created by Sam on 2024-04-04.
//

#ifndef DOUGHNUTSANDBOX_COMMAND_PIPELINE_H
#define DOUGHNUTSANDBOX_COMMAND_PIPELINE_H

#include "graphics/vulkan/context.h"
#include "graphics/vulkan/handles/command_pool.h"
#include "graphics/vulkan/handles/command_buffer.h"

#include <vector>

namespace dn::vulkan {
    struct CommandPipelineConfiguration {
        Queue queue;
        uint32_t bufferCount;
    };

    class CommandPipeline {
    public:
        CommandPipeline(Context &context, const CommandPipelineConfiguration &config);

        CommandPipeline(const CommandPipeline &other) = delete;

        CommandPipeline(CommandPipeline &&other) = default;

        ~CommandPipeline() = default;

        void nextBuffer();

        CommandBuffer *operator->();

        const CommandBuffer *operator->() const;

        CommandBuffer &operator*();

        const CommandBuffer &operator*() const;

        uint32_t mCurrentBuffer = 0;

    private:
        Context &mContext;
        CommandPipelineConfiguration mConfig;

        // Order of initialization
    public:
        CommandPool mCommandPool;
        std::vector<CommandBuffer> mCommandBuffers;
    };
}

#endif //DOUGHNUTSANDBOX_COMMAND_PIPELINE_H
