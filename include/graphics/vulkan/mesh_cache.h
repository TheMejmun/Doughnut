//
// Created by Sam on 2024-02-21.
//

#ifndef DOUGHNUT_MESH_CACHE_H
#define DOUGHNUT_MESH_CACHE_H

#include "graphics/vulkan/buffer.h"

namespace dn::vulkan {
    struct MeshReference {
        uint32_t bufferIndex{};
        BufferPosition vertexPosition{};
        vk::Buffer vertexBuffer;
        BufferPosition indexPosition{};
        vk::Buffer indexBuffer;
    };

    class MeshCache {
    public:
        explicit MeshCache(Context &context);

        MeshCache(const MeshCache &other) = delete;

        MeshCache(MeshCache &&other) = delete;

        ~MeshCache();

        void preload(const std::string &mesh);

        MeshReference &get(const std::string &mesh);

    private:
        Context &mContext;
        Buffer mVertexBuffer;
        Buffer mIndexBuffer;

        std::unordered_map<std::string, MeshReference> mMeshIndices{};
        std::mutex mInsertMeshMutex{};
    };
}

#endif //DOUGHNUT_MESH_CACHE_H
