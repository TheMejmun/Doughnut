//
// Created by Sam on 2024-01-23.
//

#ifndef DOUGHNUT_RENDER_API_H
#define DOUGHNUT_RENDER_API_H

#include "preprocessor.h"
#include "io/window.h"
#include "graphics/vulkan/optional_features.h"
#include "graphics/vulkan/queue_family_indices.h"
#include "graphics/vulkan/swapchain.h"
#include "graphics/vulkan/instance.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <string>
#include <optional>

namespace dn {
    class VulkanAPI {
    public:
        VulkanAPI(Window &window);

        ~VulkanAPI();

    private:
        void createSwapchain();

        void destroySwapchain();

        void destroyDevice();

        void destroyInstance();

        std::optional<vulkan::Instance> mInstance{};
        std::optional<vulkan::Swapchain> mSwapchain{};
    };
}

#endif //DOUGHNUT_RENDER_API_H
