//
// Created by Sam on 2024-02-21.
//

#include "graphics/vulkan/texture_cache.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

TextureCache::TextureCache(Context &context)
        : mContext(context),
          mStagingBuffer(mContext, ImageStagingBufferConfiguration{}) {
    log::d("Creating TextureCache");
}

void TextureCache::preload(const std::string &texture) {
    std::unique_lock<std::mutex> guard{mInsertTextureMutex};
    if (!mRenderTextures.contains(texture)) {
        auto textureImport = Texture(texture);
        guard.unlock();
        preload(textureImport);
    }
}

void TextureCache::preload(const Texture &texture) {
    std::lock_guard<std::mutex> guard{mInsertTextureMutex};
    if (!mRenderTextures.contains(texture.mFilename)) {

        // https://stackoverflow.com/questions/68828864/how-can-you-emplace-directly-a-mapped-value-into-an-unordered-map
        mRenderTextures.try_emplace(
                texture.mFilename,
                mContext,
                texture
        );
    }
}

RenderTexture &TextureCache::get(const std::string &texture) {
    if (!mRenderTextures.contains(texture)) {
        preload(texture);
        mStagingBuffer.awaitUpload();
    }
    return mRenderTextures.at(texture);
}

RenderTexture &TextureCache::get(const Texture &texture) {
    if (!mRenderTextures.contains(texture.mFilename)) {
        preload(texture);
        mStagingBuffer.awaitUpload();
    }
    return mRenderTextures.at(texture.mFilename);
}

TextureCache::~TextureCache() {
    log::d("Destroying TextureCache");
}