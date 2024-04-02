//
// Created by Saman on 02.04.24.
//

#ifndef ONEOVERNEGATIVE_RENDER_TEXTURE_H
#define ONEOVERNEGATIVE_RENDER_TEXTURE_H

#include "graphics/vulkan/handles/image.h"
#include "graphics/vulkan/handles/image_view.h"
#include "graphics/vulkan/handles/sampler.h"
#include "graphics/vulkan/handles/framebuffer.h"
#include "graphics/texture.h"
#include "image_staging_buffer.h"

namespace dn::vulkan {
    class RenderTexture {
    public:
        explicit RenderTexture(Context &context, const Texture &texture);

        RenderTexture(const RenderTexture &other) = delete;

        RenderTexture(RenderTexture &&other) = default;

        Context &mContext;
        Image mImage;
        ImageView mImageView;
        Sampler mSampler;
        // Framebuffer mFramebuffer; // TODO

    private:
        ImageStagingBuffer mStagingBuffer;
    };
}

#endif //ONEOVERNEGATIVE_RENDER_TEXTURE_H
