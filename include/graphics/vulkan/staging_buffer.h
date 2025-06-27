//
// Created by Sam on 2024-02-18.
//

#ifndef DOUGHNUT_STAGING_BUFFER_H
#define DOUGHNUT_STAGING_BUFFER_H

#include <vulkan/vulkan.hpp>
#include "context.h"
#include "graphics/vertex.h"
#include "core/scheduler.h"
#include "graphics/vulkan/handles/command_pool.h"
#include "graphics/vulkan/handles/command_buffer.h"
#include "core/late_init.h"
#include "graphics/vulkan/handles/fence.h"

namespace dn::vulkan {
    struct StagingBufferConfiguration {
    };

    class StagingBuffer {
    public:
        StagingBuffer(Context &context,
                      StagingBufferConfiguration config);

        StagingBuffer(const StagingBuffer &other) = delete;

        StagingBuffer(StagingBuffer &&other) = default;

        ~StagingBuffer();

        void upload(uint32_t size, const void *data, vk::Buffer target, uint32_t at);

        bool isCurrentlyUploading();

        void freeStagingMemory();

        void awaitUpload();

    private:
        Context &mContext;
        StagingBufferConfiguration mConfig;

        CommandPool mCommandPool;
        CommandBuffer mCommandBuffer;
        Fence mFence;

        vk::Buffer mStagingBuffer = nullptr;
        vk::DeviceMemory mStagingBufferMemory = nullptr;
    };
}
#endif //DOUGHNUT_STAGING_BUFFER_H
