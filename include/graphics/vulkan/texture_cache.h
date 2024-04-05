//
// Created by Sam on 2024-02-21.
//

#ifndef DOUGHNUT_TEXTURE_CACHE_H
#define DOUGHNUT_TEXTURE_CACHE_H

#include "graphics/vulkan/context.h"
#include "graphics/vulkan/handles/image.h"
#include "graphics/vulkan/handles/image_view.h"
#include "graphics/vulkan/image_staging_buffer.h"
#include "graphics/vulkan/render_texture.h"

#include <string>

namespace dn::vulkan {
    class TextureCache {
    public:
        explicit TextureCache(Context &context);

        ~TextureCache();

        void preload(const std::string &texture);

        void preload(const Texture &texture);

        RenderTexture &get(const std::string &texture);

        RenderTexture &get(const Texture &texture);

    private:
        Context &mContext;
        ImageStagingBuffer mStagingBuffer;
        std::unordered_map<std::string, RenderTexture> mRenderTextures{};
        std::mutex mInsertTextureMutex{};
    };
}
#endif //DOUGHNUT_TEXTURE_CACHE_H
