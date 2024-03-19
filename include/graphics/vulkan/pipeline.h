//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUT_PIPELINE_H
#define DOUGHNUT_PIPELINE_H

#include "descriptor_set_layout.h"
#include "core/late_init.h"
#include "graphics/vulkan/render_pass.h"
#include "descriptor_pool.h"
#include "descriptor_set.h"

#include <vulkan/vulkan.hpp>
#include <optional>

namespace dn::vulkan {
    // Warning: buffer and texture references are not kept track of inside PipelineCache
    struct PipelineConfiguration {
        std::string vertexShader;
        std::string fragmentShader;
        DescriptorSetConfiguration descriptorSetConfig;
        bool wireFrameMode = false;
    };

    class Pipeline {
    public:
        Pipeline(Context &context,
                 RenderPass &renderPass,
                 const PipelineConfiguration &config);

        Pipeline(Pipeline &&other) noexcept;

        ~Pipeline();

        vk::PipelineLayout mPipelineLayout = nullptr;
        vk::Pipeline mGraphicsPipeline = nullptr;

        LateInit<DescriptorSetLayout> mDescriptorSetLayout{};
        LateInit<DescriptorPool> mDescriptorPool{};
        LateInit<DescriptorSet> mDescriptorSet{};
    private:
        Context &mContext;
    };
}

#endif //DOUGHNUT_PIPELINE_H
