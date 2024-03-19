//
// Created by Sam on 2024-02-21.
//

#include "graphics/vulkan/mesh_cache.h"
#include "io/logger.h"
#include "util/importer.h"

using namespace dn;
using namespace dn::vulkan;

MeshCache::MeshCache(Context &context)
        : mContext(context),
          mVertexBuffer(mContext,
                        BufferConfiguration{VERTEX, false}),
          mIndexBuffer(mContext,
                       BufferConfiguration{INDEX, false}) {
    log::d("Creating MeshCache");
}

void MeshCache::preload(const std::string &mesh) {
    std::lock_guard<std::mutex> guard{mInsertMeshMutex};
    if (!mMeshIndices.contains(mesh)) {
        auto meshImport = importMesh(mesh);

        auto vertexPosition = mVertexBuffer.queueUpload(meshImport.vertices);
        auto indexPosition = mIndexBuffer.queueUpload(meshImport.indices);
        mVertexBuffer.awaitUpload();
        mIndexBuffer.awaitUpload();

        mMeshIndices.emplace(
                mesh,
                MeshReference{
                        0,
                        vertexPosition.position,
                        mVertexBuffer.mBuffer,
                        indexPosition.position,
                        mIndexBuffer.mBuffer
                }
        );
    }
}

MeshReference &MeshCache::get(const std::string &mesh) {
    preload(mesh);
    return mMeshIndices.at(mesh);
}

MeshCache::~MeshCache() {
    log::d("Destroying MeshCache");
}