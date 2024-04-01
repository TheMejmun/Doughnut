//
// Created by Sam on 2024-02-09.
//

#ifndef DOUGHNUT_IMAGE_VIEW_H
#define DOUGHNUT_IMAGE_VIEW_H

#include "image.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct ImageViewConfiguration {
        vk::Extent2D extent;
        vk::Format format;
        vk::ImageAspectFlags aspectFlags = {vk::ImageAspectFlagBits::eColor};
    };

    class ImageView : public Handle<vk::ImageView, ImageViewConfiguration> {
    public:
        ImageView(Context &context,
                  const Image &image,
                  const ImageViewConfiguration &config);

        ImageView(ImageView &&other) = default;

        ~ImageView();
    };
}

#endif //DOUGHNUT_IMAGE_VIEW_H
