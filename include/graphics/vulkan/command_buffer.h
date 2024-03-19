//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_COMMAND_BUFFER_H
#define DOUGHNUT_COMMAND_BUFFER_H

#include "context.h"
#include "command_pool.h"
#include "handle.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct CommandBufferConfiguration {
    };

    class CommandBuffer : public Handle<vk::CommandBuffer, CommandBufferConfiguration> {
    public:
        CommandBuffer(Context &context,
                      CommandPool &pool,
                      const CommandBufferConfiguration &config);

        CommandBuffer(CommandBuffer &&other) = default;

        ~CommandBuffer() = default; // Not necessary. Pool destruction will handle this

        void reset() const;

        void startRecording();

        void endRecording();

        bool mIsRecording = false;
    };
}

#endif //DOUGHNUT_COMMAND_BUFFER_H
