//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUTSANDBOX_PIPELINE_H
#define DOUGHNUTSANDBOX_PIPELINE_H

#include "descriptor_set_layout.h"
#include "core/late_init.h"
#include "graphics/vulkan/render_pass.h"
#include "descriptor_pool.h"
#include "descriptor_set.h"

#include <vulkan/vulkan.hpp>
#include <optional>

namespace dn::vulkan {
    struct PipelineConfiguration {

    };

    class Pipeline {
    public:
        Pipeline(Instance &instance,
                 RenderPass &renderPass,
                 Buffer &uboBuffer,
                 PipelineConfiguration config);

        Pipeline(Pipeline &&other) noexcept;

        ~Pipeline();

        vk::PipelineLayout mPipelineLayout = nullptr;
        vk::Pipeline mGraphicsPipeline = nullptr;

        LateInit<DescriptorSetLayout> mDescriptorSetLayout{};
        LateInit<DescriptorPool> mDescriptorPool{};
        LateInit<DescriptorSet> mDescriptorSet{};
    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUTSANDBOX_PIPELINE_H
