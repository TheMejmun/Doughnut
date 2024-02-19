//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUTSANDBOX_BUFFER_H
#define DOUGHNUTSANDBOX_BUFFER_H

#include "instance.h"
#include "graphics/vertex.h"
#include "core/scheduler.h"
#include "staging_buffer.h"

#include <vulkan/vulkan.hpp>

// TODO use new semaphore, fence, command buffer classes

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

    struct BufferPosition {
        uint32_t memoryIndex;
        uint32_t size;
        uint32_t count;
    };

    // TODO consider memory alignment
    struct UploadResult {
        bool notEnoughSpace;
        BufferPosition position;
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
            UploadResult result = queueUpload(sizeof(T) * data.size(), reinterpret_cast<const uint8_t *>(data.data()));
            result.position.count = data.size();
            return result;
        }

        template<class T>
        void queueUpload(const std::vector<T> &data, uint32_t at) {
            queueUpload(sizeof(T) * data.size(), reinterpret_cast<const uint8_t *>(data.data()), at);
        }

        template<class T>
        UploadResult directUpload(const std::vector<T> &data) {
            UploadResult result = directUpload(sizeof(T) * data.size(), reinterpret_cast<const uint8_t *>(data.data()));
            result.position.count = data.size();
            return result;
        }

        template<class T>
        void directUpload(const std::vector<T> &data, uint32_t at) {
            directUpload(sizeof(T) * data.size(), reinterpret_cast<const uint8_t *>(data.data()), at);
        }

        UploadResult reserve(uint32_t size);

        bool isCurrentlyUploading() const;

        void freeStagingMemory();

        void awaitUpload();

        vk::Buffer mBuffer = nullptr;

    private:
        UploadResult calculateMemoryIndex(uint32_t size);

        UploadResult queueUpload(uint32_t size, const uint8_t *data);

        void queueUpload(uint32_t size, const uint8_t *data, uint32_t at);

        UploadResult directUpload(uint32_t size, const uint8_t *data);

        void directUpload(uint32_t size, const uint8_t *data, uint32_t at);

        Instance &mInstance;
        BufferConfiguration mConfig;

        LateInit<StagingBuffer> mStagingBuffer{};

        vk::DeviceMemory mBufferMemory = nullptr;
        uint8_t *mMappedBuffer = nullptr;

        std::vector<bool> mIsUsed{};
        std::unique_ptr<std::mutex> mIsUsedMutex{};
    };
}

#endif //DOUGHNUTSANDBOX_BUFFER_H
