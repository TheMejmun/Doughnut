//
// Created by Sam on 2024-02-21.
//

#ifndef DOUGHNUT_TEXTURE_CACHE_H
#define DOUGHNUT_TEXTURE_CACHE_H

#include "graphics/vulkan/context.h"
#include "graphics/vulkan/image.h"
#include "graphics/vulkan/image_view.h"
#include "graphics/vulkan/image_staging_buffer.h"

#include <string>

namespace dn::vulkan {
    class TextureCache {
    public:
        explicit TextureCache(Context &context);

        ~TextureCache();

        void preload(const std::string &texture);

        Image &getImage(const std::string &texture);

        ImageView &getImageView(const std::string &texture);

    private:
        Context &mContext;
        ImageStagingBuffer mStagingBuffer;
        std::unordered_map<std::string, Image> mImages{};
        std::unordered_map<std::string, ImageView> mImageViews{};
        std::mutex mInsertTextureMutex{};
    };
}
#endif //DOUGHNUT_TEXTURE_CACHE_H
