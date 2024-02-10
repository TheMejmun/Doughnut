//
// Created by Sam on 2024-02-09.
//

#ifndef DOUGHNUTSANDBOX_IMAGE_VIEW_H
#define DOUGHNUTSANDBOX_IMAGE_VIEW_H

#include "image.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct ImageViewConfiguration {
        vk::Extent2D extent;
        vk::Format format;
        vk::ImageAspectFlags aspectFlags = {vk::ImageAspectFlagBits::eColor};
    };

    class ImageView {
    public:
        ImageView(Instance &instance,
                  const Image &image,
                  ImageViewConfiguration config);

        ImageView(ImageView &&other) noexcept;

        ~ImageView();

        vk::ImageView mImageView;
        vk::Extent2D mExtent{};

    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUTSANDBOX_IMAGE_VIEW_H
