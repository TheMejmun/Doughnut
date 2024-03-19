//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_COMMAND_BUFFER_H
#define DOUGHNUT_COMMAND_BUFFER_H

#include "context.h"
#include "command_pool.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    class CommandBuffer {
    public:
        CommandBuffer(Context &context,
                      CommandPool &pool);

        CommandBuffer(CommandBuffer &&other) noexcept;

        ~CommandBuffer();

        vk::CommandBuffer mCommandBuffer = nullptr;

        void reset() const;

        void startRecording();

        void endRecording();

        bool mIsRecording = false;

    private:
        Context &mContext;
    };
}

#endif //DOUGHNUT_COMMAND_BUFFER_H
