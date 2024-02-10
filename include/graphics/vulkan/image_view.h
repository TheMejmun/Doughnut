//
// Created by Sam on 2024-02-09.
//

#ifndef DOUGHNUTSANDBOX_IMAGE_VIEW_H
#define DOUGHNUTSANDBOX_IMAGE_VIEW_H

#include "image.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct ImageViewConfiguration {
        vk::Format format;
        vk::ImageAspectFlags aspectFlags = {vk::ImageAspectFlagBits::eColor};
    };

    class ImageView {
    public:
        ImageView(vk::Device device,
                  const Image &image,
                  const vk::Extent2D &extent,
                  ImageViewConfiguration config);

        ImageView(ImageView &&other) noexcept;

        ~ImageView();

        vk::ImageView mImageView;
        vk::Extent2D mExtent{};

    private:
        vk::Device mDevice;
    };
}

#endif //DOUGHNUTSANDBOX_IMAGE_VIEW_H
