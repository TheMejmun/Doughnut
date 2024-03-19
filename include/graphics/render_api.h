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
#include "graphics/vulkan/context.h"
#include "core/late_init.h"
#include "graphics/vulkan/pipeline.h"
#include "graphics/vulkan/buffer.h"
#include "graphics/vulkan/command_pool.h"
#include "graphics/vulkan/command_buffer.h"
#include "graphics/vulkan/semaphore.h"
#include "graphics/vulkan/fence.h"
#include "graphics/vulkan/pipeline_cache.h"
#include "graphics/vulkan/mesh_cache.h"
#include "graphics/vulkan/texture_cache.h"
#include "renderable.h"
#include "graphics/vulkan/sampler.h"
#include "graphics/vulkan/gui.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <string>
#include <optional>

namespace dn {
    class VulkanAPI {
    public:
        explicit VulkanAPI(Window &window);

        ~VulkanAPI();

        bool nextImage();

        void startRecording();

        void beginRenderPass();

        void recordDraw(const Renderable &renderable);

        void recordUiDraw();

        void endRenderPass();

        void endRecording();

        void drawFrame(double delta);

        LateInit<vulkan::Buffer> mUniformBuffer{};

    private:
        LateInit<vulkan::Context> mContext{};
        LateInit<vulkan::Swapchain> mSwapchain{};
        LateInit<vulkan::PipelineCache> mPipelines{};
        LateInit<vulkan::CommandPool> mCommandPool{};
        std::vector<vulkan::CommandBuffer> mCommandBuffers{};
        LateInit<vulkan::Semaphore> mImageAvailableSemaphore{};
        LateInit<vulkan::Semaphore> mRenderFinishedSemaphore{};
        LateInit<vulkan::Fence> mInFlightFence{};
        LateInit<vulkan::Sampler> mSampler{};

        std::optional<uint32_t> mCurrentSwapchainFramebuffer{};

        LateInit<vulkan::MeshCache> mMeshes{};
        LateInit<vulkan::TextureCache> mTextures{};

        LateInit<vulkan::Gui> mGui{};
    };
}

#endif //DOUGHNUT_RENDER_API_H
