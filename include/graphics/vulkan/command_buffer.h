//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_COMMAND_BUFFER_H
#define DOUGHNUT_COMMAND_BUFFER_H

#include "instance.h"
#include "command_pool.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    class CommandBuffer {
    public:
        CommandBuffer(Instance &instance,
                      CommandPool &pool);

        CommandBuffer(CommandBuffer &&other) noexcept;

        ~CommandBuffer();

        vk::CommandBuffer mCommandBuffer = nullptr;

        void reset() const;

        void startRecording();

        void endRecording();

        bool mIsRecording = false;

    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUT_COMMAND_BUFFER_H
