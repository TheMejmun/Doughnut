//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUTSANDBOX_BUFFER_H
#define DOUGHNUTSANDBOX_BUFFER_H

#include <vulkan/vulkan.hpp>
#include "instance.h"
#include "graphics/vertex.h"
#include "core/scheduler.h"

namespace dn::vulkan {
    enum BufferType {
        INDEX,
        VERTEX,
        UNIFORM
    };

    struct BufferConfiguration {
        BufferType type;
        bool hostDirectAccessible;
    };

    struct UploadResult {
        bool notEnoughSpace;
        uint32_t memoryIndex;
    };

    class Buffer {
    public:
        Buffer(Instance &instance,
               BufferConfiguration config);

        Buffer(Buffer &&other) noexcept;

        ~Buffer();

        // TODO Function to delete data

        /**
         * This function will wait for existing uploads to finish first.
         * @param data The data to upload.
         * @return The result of this operation.
         */
        template<class T>
        UploadResult queueUpload(const std::vector<T> &data) {
            return upload(sizeof(T) * data.size(), reinterpret_cast<const uint8_t *>(data.data()));
        }

        template<class T>
        UploadResult directUpload(const std::vector<T> &data) {
            return upload(sizeof(T) * data.size(), reinterpret_cast<const uint8_t *>(data.data()));
        }

        bool isCurrentlyUploading();

        void freeStagingMemory();

    private:
        UploadResult calculateMemoryIndex(uint32_t size);

        UploadResult queueUpload(uint32_t size, const uint8_t *data);

        UploadResult directUpload(uint32_t size, const uint8_t *data);

        Instance &mInstance;
        BufferConfiguration mConfig;

        vk::CommandPool mTransferCommandPool = nullptr;
        vk::CommandBuffer mTransferCommandBuffer = nullptr;
        vk::Fence mTransferFence = nullptr;

        vk::Buffer mStagingBuffer = nullptr;
        vk::DeviceMemory mStagingBufferMemory = nullptr;

        vk::Buffer mBuffer = nullptr;
        vk::DeviceMemory mBufferMemory = nullptr;
        uint8_t *mMappedBuffer = nullptr;

        std::vector<bool> mIsUsed{};
        std::unique_ptr<std::mutex> mIsUsedMutex{};
    };
}

#endif //DOUGHNUTSANDBOX_BUFFER_H
