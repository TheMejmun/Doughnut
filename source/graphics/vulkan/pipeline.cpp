//
// Created by Sam on 2024-02-11.
//

#include "graphics/vulkan/pipeline.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

Pipeline::Pipeline(Instance &instance, PipelineConfiguration config) : mInstance(instance) {
    log::v("Creating Pipeline");
    DescriptorSetLayoutConfiguration layoutConfig{
            std::vector<DescriptorSetConfiguration>{
                    DescriptorSetConfiguration{}
            }
    };

    mDescriptorSetLayout.emplace(
            mInstance,
            layoutConfig
    );
}

// TODO will this actually move the other Pipeline's descriptor set layout over without destroying it?
Pipeline::Pipeline(dn::vulkan::Pipeline &&other) noexcept
        : mInstance(other.mInstance), mDescriptorSetLayout(std::exchange(other.mDescriptorSetLayout, nullptr)) {
    log::v("Moving Pipeline");
}

Pipeline::~Pipeline() {
    log::v("Destroying Pipeline");
    mDescriptorSetLayout.reset();
}