//
// Created by Sam on 2024-02-19.
//

#ifndef DOUGHNUTSANDBOX_IMAGE_STAGING_BUFFER_H
#define DOUGHNUTSANDBOX_IMAGE_STAGING_BUFFER_H

#include <vulkan/vulkan.hpp>
#include "context.h"
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
        ImageStagingBuffer(Context &context,
                           ImageStagingBufferConfiguration config);

        ImageStagingBuffer(const ImageStagingBuffer &other) = delete;

        ImageStagingBuffer(ImageStagingBuffer &&other) = default;

        ~ImageStagingBuffer();

        void upload(const Texture &texture, vk::Image target);

        bool isCurrentlyUploading();

        void freeStagingMemory();

        void awaitUpload();

    private:
        Context &mContext;
        ImageStagingBufferConfiguration mConfig;

        CommandPool mCommandPool;
        CommandBuffer mCommandBuffer;
        Fence mFence;

        // TODO use dn Handles
        vk::Buffer mStagingBuffer = nullptr;
        vk::DeviceMemory mStagingBufferMemory = nullptr;
    };
}
#endif //DOUGHNUTSANDBOX_IMAGE_STAGING_BUFFER_H
