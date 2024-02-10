//
// Created by Sam on 2024-02-09.
//

#include "graphics/vulkan/image_view.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

ImageView::ImageView(vk::Device device,
                     const Image &image,
                     ImageViewConfiguration config)
        : mDevice(device) {
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

    mImageView = device.createImageView(viewCreateInfo);
}

ImageView::ImageView(ImageView &&other) noexcept
        : mImageView(std::exchange(other.mImageView, nullptr)), mDevice(other.mDevice) {
    log::v("Moving ImageView");
}

ImageView::~ImageView() {
    log::v("Destroying ImageView");
    if (mImageView != nullptr) { mDevice.destroy(mImageView); }
}