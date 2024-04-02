//
// Created by Saman on 02.04.24.
//

#include "graphics/vulkan/render_texture.h"

using namespace dn;
using namespace dn::vulkan;

RenderTexture::RenderTexture(Context &context, const dn::Texture &texture)
        : mContext(context),
          mImage(mContext,
                 ImageConfiguration{
                         {texture.mWidth, texture.mHeight},
                         false,
                         true,
                         texture.mLayout
                 }),
          mImageView(mContext,
                     mImage,
                     ImageViewConfiguration{
                             {texture.mWidth, texture.mHeight},
                             mImage.mFormat,
                             vk::ImageAspectFlagBits::eColor // TODO only if texture is not mono
                     }),
          mSampler(mContext,
                   SamplerConfiguration{
                           CLAMP
                   }),
          mStagingBuffer(mContext,
                         ImageStagingBufferConfiguration{}) {
    mStagingBuffer.upload(texture, *mImage);
    mStagingBuffer.awaitUpload(); // TODO may be able to do this in a non-blocking way
    log::v(texture.mFilename, "upload completed");
}

