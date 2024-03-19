//
// Created by Sam on 2024-02-11.
//

#include "graphics/vulkan/pipeline.h"
#include "io/logger.h"
#include "util/importer.h"
#include "graphics/vulkan/shader_module.h"
#include "util/require.h"
#include "graphics/vulkan/buffer.h"
#include "graphics/uniform_buffer_object.h"
#include "graphics/push_constants_object.h"

using namespace dn;
using namespace dn::vulkan;

Pipeline::Pipeline(Context &context,
                   RenderPass &renderPass,
                   const PipelineConfiguration &config)
        : mContext(context) {
    log::d("Creating Pipeline");

    // https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions

    DescriptorSetLayoutConfiguration layoutConfig{
            // TODO
    };

    mDescriptorSetLayout.emplace(
            mContext,
            layoutConfig
    );

    // TODO pull these out of here
    ShaderModule vertexShader = ShaderModule{mContext, config.vertexShader};
    ShaderModule fragmentShader = ShaderModule{mContext, config.fragmentShader};

    vk::PipelineShaderStageCreateInfo vertexShaderStageInfo{
            {},
            vk::ShaderStageFlagBits::eVertex,
            vertexShader.mShaderModule,
            "main",
            nullptr // Use pSpecializationInfo to specify constants
    };

    vk::PipelineShaderStageCreateInfo fragmentShaderStageInfo{
            {},
            vk::ShaderStageFlagBits::eFragment,
            fragmentShader.mShaderModule,
            "main",
            nullptr // Use pSpecializationInfo to specify constants
    };

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {vertexShaderStageInfo, fragmentShaderStageInfo};

    // TODO more configurable Setup
    const auto bindingDescription = Vertex::getBindingDescription();
    const auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
            {},
            1,
            &bindingDescription,
            static_cast<uint32_t>(attributeDescriptions.size()),
            attributeDescriptions.data(),
    };

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
            {},
            vk::PrimitiveTopology::eTriangleList,
            vk::False
    };

    vk::PipelineViewportStateCreateInfo viewportState{
            {},
            1,
            nullptr, // For dynamic state?
            1,
            nullptr, // For dynamic state?
    };

    // Optional dynamic state
    std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{
            {},
            static_cast<uint32_t>(dynamicStates.size()),
            dynamicStates.data()
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer{
            {},
            vk::False,
            vk::False,
            (mContext.mOptionalFeatures.supportsWireframeMode && config.wireFrameMode)
            ? vk::PolygonMode::eLine
            : vk::PolygonMode::eFill,
            vk::CullModeFlags{vk::CullModeFlagBits::eBack},
            vk::FrontFace::eCounterClockwise,
            vk::False,
            0.0f,
            0.0f,
            0.0f,
            1.0f,
    };

    // Requires a GPU feature
    vk::PipelineMultisampleStateCreateInfo multisampling{
            {},
            vk::SampleCountFlagBits::e1,
            vk::False,
            1.0f,
            nullptr,
            vk::False,
            vk::False,
    };

    // Depth or Stencil testing: vk::PipelineDepthStencilStateCreateInfo

    // Color blend info per attached Framebuffer
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
            vk::False,
            vk::BlendFactor::eOne,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::BlendFactor::eOne,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::ColorComponentFlags{
                    vk::ColorComponentFlagBits::eR |
                    vk::ColorComponentFlagBits::eG |
                    vk::ColorComponentFlagBits::eB |
                    vk::ColorComponentFlagBits::eA
            }
    };

    // Global Color blending
    vk::PipelineColorBlendStateCreateInfo colorBlending{
            {},
            vk::False,
            vk::LogicOp::eCopy,
            1,
            &colorBlendAttachment,
            std::array<float, 4>{
                    0.0f, 0.0f, 0.0f, 0.0f
            },
    };

    vk::PushConstantRange pushConstantRange{
            vk::ShaderStageFlagBits::eAll,
            0,
            sizeof(PushConstantsObject)
    };

    // Define uniforms
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
            {},
            1,
            &mDescriptorSetLayout->mLayout,
            1,
            &pushConstantRange,
    };

    mPipelineLayout = mContext.mDevice.createPipelineLayout(pipelineLayoutInfo);

    vk::PipelineDepthStencilStateCreateInfo depthStencil{
            {},
            vk::True,
            vk::True,
            vk::CompareOp::eLess,
            vk::False,
            vk::False,
            {},
            {},
            0.0f,
            1.0f,
    };

    vk::GraphicsPipelineCreateInfo pipelineInfo{
            {},
            shaderStages.size(),
            shaderStages.data(),
            &vertexInputInfo,
            &inputAssembly,
            nullptr,
            &viewportState,
            &rasterizer,
            &multisampling,
            &depthStencil,
            &colorBlending,
            &dynamicState,
            mPipelineLayout,
            renderPass.mRenderPass,
            0,
            nullptr,
            -1,
    };

    auto result = mContext.mDevice.createGraphicsPipeline(nullptr, pipelineInfo);
    require(result.result, "Failed to create graphics pipeline");
    mGraphicsPipeline = result.value;

    mDescriptorPool.emplace(
            mContext,
            DescriptorPoolConfiguration{
                    2u,
                    {
                            {UNIFORM_BUFFER, 1},
                            {SAMPLER, 1}
                    }
            }
    );

    mDescriptorSet.emplace(
            mContext,
            *mDescriptorSetLayout,
            *mDescriptorPool,
            config.descriptorSetConfig
    );
}

// TODO will this actually move the other Pipeline's descriptor set layout over without destroying it?
Pipeline::Pipeline(dn::vulkan::Pipeline &&other) noexcept
        : mContext(other.mContext),
          mDescriptorSetLayout(std::exchange(other.mDescriptorSetLayout, nullptr)),
          mPipelineLayout(std::exchange(other.mPipelineLayout, nullptr)),
          mGraphicsPipeline(std::exchange(other.mGraphicsPipeline, nullptr)),
          mDescriptorPool(std::exchange(other.mDescriptorPool, nullptr)),
          mDescriptorSet(std::exchange(other.mDescriptorSet, nullptr)) {
    log::d("Moving Pipeline");
}

Pipeline::~Pipeline() {
    log::d("Destroying Pipeline");
    mDescriptorSet.reset();
    mDescriptorPool.reset();
    if (mGraphicsPipeline != nullptr) { mContext.mDevice.destroy(mGraphicsPipeline); }
    if (mPipelineLayout != nullptr) { mContext.mDevice.destroy(mPipelineLayout); }
    mDescriptorSetLayout.reset();
}