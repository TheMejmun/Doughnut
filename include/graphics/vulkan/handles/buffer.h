//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUT_BUFFER_H
#define DOUGHNUT_BUFFER_H

#include "graphics/vulkan/context.h"
#include "graphics/vertex.h"
#include "core/scheduler.h"
#include "graphics/vulkan/staging_buffer.h"
#include "core/late_init.h"
#include "graphics/vulkan/handle.h"

#include <vulkan/vulkan.hpp>

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

    class Buffer : public Handle<vk::Buffer, BufferConfiguration> {
    public:
        Buffer(Context &context,
               BufferConfiguration config);

        Buffer(Buffer &&other) = default;

        ~Buffer();

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

        void clear(uint32_t at, uint32_t byteSize);

        inline void clear() {
            clear(0, mIsUsed.size());
        }

        inline void clear(const BufferPosition &position) {
            clear(position.memoryIndex, position.size);
        }

        inline void clear(const UploadResult &position) {
            clear(position.position.memoryIndex, position.position.size);
        }

        UploadResult reserve(uint32_t size);

        bool isCurrentlyUploading();

        void freeStagingMemory();

        void awaitUpload();

    private:
        UploadResult calculateMemoryIndex(uint32_t size);

        UploadResult queueUpload(uint32_t size, const uint8_t *data);

        void queueUpload(uint32_t size, const uint8_t *data, uint32_t at);

        UploadResult directUpload(uint32_t size, const uint8_t *data);

        void directUpload(uint32_t size, const uint8_t *data, uint32_t at);

        LateInit<StagingBuffer> mStagingBuffer{};

        vk::DeviceMemory mBufferMemory = nullptr;
        uint8_t *mMappedBuffer = nullptr;

        std::vector<bool> mIsUsed{};
        std::unique_ptr<std::mutex> mIsUsedMutex{};
    };
}

#endif //DOUGHNUT_BUFFER_H
