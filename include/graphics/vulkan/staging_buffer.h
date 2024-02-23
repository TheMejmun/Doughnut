//
// Created by Sam on 2024-02-18.
//

#ifndef DOUGHNUT_STAGING_BUFFER_H
#define DOUGHNUT_STAGING_BUFFER_H

#include <vulkan/vulkan.hpp>
#include "instance.h"
#include "graphics/vertex.h"
#include "core/scheduler.h"
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

        StagingBuffer(StagingBuffer &&other) noexcept;

        ~StagingBuffer();

        void upload(uint32_t size, const void *data, vk::Buffer target, uint32_t at);

        bool isCurrentlyUploading();

        void freeStagingMemory();

        void awaitUpload();

    private:
        Instance &mInstance;
        StagingBufferConfiguration mConfig;

        LateInit<CommandPool> mCommandPool;
        LateInit<CommandBuffer> mCommandBuffer;
        LateInit<Fence> mFence;

        vk::Buffer mStagingBuffer = nullptr;
        vk::DeviceMemory mStagingBufferMemory = nullptr;
    };
}
#endif //DOUGHNUT_STAGING_BUFFER_H
