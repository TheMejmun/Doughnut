//
// Created by Sam on 2024-02-09.
//

#include "graphics/vulkan/image_view.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

ImageView::ImageView(Context &context,
                     const Image &image,
                     const ImageViewConfiguration &config)
        : Handle<vk::ImageView, ImageViewConfiguration>(context, config) {

    vk::ImageViewCreateInfo viewCreateInfo{
            {},
            *image,
            vk::ImageViewType::e2D,
            mConfig.format,
            vk::ComponentMapping{
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
            },
            vk::ImageSubresourceRange{
                    mConfig.aspectFlags,
                    0,
                    1,
                    0,
                    1
            }
    };

    mVulkan = mContext.mDevice.createImageView(viewCreateInfo);
}

ImageView::~ImageView() {
    if (mVulkan != nullptr) {
        mContext.mDevice.destroy(mVulkan);
    }
}