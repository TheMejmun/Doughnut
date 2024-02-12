//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUTSANDBOX_COMMAND_BUFFER_H
#define DOUGHNUTSANDBOX_COMMAND_BUFFER_H

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

    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUTSANDBOX_COMMAND_BUFFER_H
