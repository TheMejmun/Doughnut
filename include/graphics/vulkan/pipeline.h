//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUTSANDBOX_PIPELINE_H
#define DOUGHNUTSANDBOX_PIPELINE_H

#include "descriptor_set.h"
#include "core/late_init.h"
#include "graphics/vulkan/render_pass.h"

#include <vulkan/vulkan.hpp>
#include <optional>

namespace dn::vulkan {
    struct PipelineConfiguration {

    };

    class Pipeline {
    public:
        Pipeline(Instance &instance, RenderPass &renderPass, PipelineConfiguration config);

        Pipeline(Pipeline &&other) noexcept;

        ~Pipeline();

    private:
        Instance &mInstance;
        LateInit<DescriptorSetLayout> mDescriptorSetLayout{};
        vk::PipelineLayout mPipelineLayout = nullptr;
        vk::Pipeline mGraphicsPipeline = nullptr;
    };
}

#endif //DOUGHNUTSANDBOX_PIPELINE_H
