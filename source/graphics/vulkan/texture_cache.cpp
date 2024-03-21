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
    std::lock_guard<std::mutex> guard{mInsertTextureMutex};
    if (!mImages.contains(texture) || !mImageViews.contains(texture)) {
        auto textureImport = Texture(texture);
        vk::Extent2D extent{static_cast<uint32_t>(textureImport.mWidth), static_cast<uint32_t>(textureImport.mHeight)};

        // https://stackoverflow.com/questions/68828864/how-can-you-emplace-directly-a-mapped-value-into-an-unordered-map
        mImages.try_emplace(
                texture,
                mContext,
                ImageConfiguration{
                        extent,
                        false,
                        true,
                        true,
                        textureImport.mHasAlpha
                }
        );
        Image &image = mImages.at(texture);

        mStagingBuffer.upload(textureImport, *image);

        mImageViews.emplace(
                texture,
                ImageView{
                        mContext,
                        image,
                        ImageViewConfiguration{
                                extent,
                                image.mFormat,
                                vk::ImageAspectFlagBits::eColor
                        }
                }
        );

        mStagingBuffer.awaitUpload();
    }
}

Image &TextureCache::getImage(const std::string &texture) {
    preload(texture);
    return mImages.at(texture);
}

ImageView &TextureCache::getImageView(const std::string &texture) {
    preload(texture);
    return mImageViews.at(texture);
}

TextureCache::~TextureCache() {
    log::d("Destroying TextureCache");
}