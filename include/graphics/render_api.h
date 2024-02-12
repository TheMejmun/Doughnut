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
#include "core/late_init.h"
#include "graphics/vulkan/pipeline.h"
#include "graphics/vulkan/buffer.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <string>
#include <optional>

namespace dn {
    class VulkanAPI {
    public:
        explicit VulkanAPI(Window &window);

        ~VulkanAPI();

    private:
        LateInit<vulkan::Instance> mInstance{};
        LateInit<vulkan::Swapchain> mSwapchain{};
        LateInit<vulkan::Pipeline> mPipeline{};
        LateInit<vulkan::Buffer> mVertexBuffer{};
        LateInit<vulkan::Buffer> mIndexBuffer{};
        LateInit<vulkan::Buffer> mUniformBuffer{};
    };
}

#endif //DOUGHNUT_RENDER_API_H
