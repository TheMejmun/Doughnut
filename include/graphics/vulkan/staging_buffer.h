//
// Created by Sam on 2024-02-18.
//

#ifndef DOUGHNUTSANDBOX_STAGING_BUFFER_H
#define DOUGHNUTSANDBOX_STAGING_BUFFER_H

#include <vulkan/vulkan.hpp>
#include "instance.h"
#include "graphics/vertex.h"
#include "core/scheduler.h"
#include "buffer.h"
#include "command_pool.h"
#include "command_buffer.h"
#include "core/late_init.h"
#include "fence.h"

namespace dn::vulkan {
    struct StagingBufferConfiguration {
    };

    class StagingBuffer {
    public:
        StagingBuffer(Instance &instance,
                      StagingBufferConfiguration config);

        StagingBuffer(const StagingBuffer &other) = delete;

        StagingBuffer(StagingBuffer &&other) = delete;

        ~StagingBuffer();

        void uploadCommand(uint32_t size, void *data, vk::Buffer target, uint32_t at);

        CommandBuffer getCommandBuffer();

        void submit();

        bool isCurrentlyUploading();

        void freeStagingMemory();

        void awaitUpload();

    private:
        void startCommandBuffer();

        Instance &mInstance;
        StagingBufferConfiguration mConfig;

        CommandPool mCommandPool;
        CommandBuffer mCommandBuffer;
        Fence mFence;

        vk::Buffer mStagingBuffer = nullptr;
        vk::DeviceMemory mStagingBufferMemory = nullptr;
    };
}
#endif //DOUGHNUTSANDBOX_STAGING_BUFFER_H
