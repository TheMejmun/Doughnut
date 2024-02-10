//
// Created by Sam on 2024-02-09.
//

#include "graphics/vulkan/image_view.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

ImageView::ImageView(Instance &instance,
                     const Image &image,
                     ImageViewConfiguration config)
        : mInstance(instance), mExtent(config.extent) {
    log::v("Creating ImageView");

    vk::ImageViewCreateInfo viewCreateInfo{
            {},
            image.mImage,
            vk::ImageViewType::e2D,
            config.format,
            vk::ComponentMapping{
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
            },
            vk::ImageSubresourceRange{
                    config.aspectFlags,
                    0,
                    1,
                    0,
                    1
            }
    };

    mImageView = mInstance.mDevice.createImageView(viewCreateInfo);
}

ImageView::ImageView(ImageView &&other) noexcept
        : mImageView(std::exchange(other.mImageView, nullptr)),mInstance(other.mInstance), mExtent(other.mExtent) {
    log::v("Moving ImageView");
}

ImageView::~ImageView() {
    log::v("Destroying ImageView");
    if (mImageView != nullptr) { mInstance.mDevice.destroy(mImageView); }
}