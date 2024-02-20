//
// Created by Sam on 2024-02-19.
//

#ifndef DOUGHNUTSANDBOX_IMAGE_STAGING_BUFFER_H
#define DOUGHNUTSANDBOX_IMAGE_STAGING_BUFFER_H

#include <vulkan/vulkan.hpp>
#include "instance.h"
#include "graphics/vertex.h"
#include "core/scheduler.h"
#include "command_pool.h"
#include "command_buffer.h"
#include "core/late_init.h"
#include "fence.h"
#include "graphics/texture.h"

namespace dn::vulkan {
    struct ImageStagingBufferConfiguration {
    };

    class ImageStagingBuffer {
    public:
        ImageStagingBuffer(Instance &instance,
                           ImageStagingBufferConfiguration config);

        ImageStagingBuffer(ImageStagingBuffer &&other) noexcept;

        ~ImageStagingBuffer();

        void upload(const Texture& texture, vk::Buffer target);

        bool isCurrentlyUploading();

        void freeStagingMemory();

        void awaitUpload();

    private:
        Instance &mInstance;
        ImageStagingBufferConfiguration mConfig;

        LateInit<CommandPool> mCommandPool;
        LateInit<CommandBuffer> mCommandBuffer;
        LateInit<Fence> mFence;

        vk::Buffer mStagingBuffer = nullptr;
        vk::DeviceMemory mStagingBufferMemory = nullptr;
    };
}
#endif //DOUGHNUTSANDBOX_IMAGE_STAGING_BUFFER_H
