//
// Created by Sam on 2024-02-21.
//

#include "graphics/vulkan/pipeline_cache.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

PipelineCache::PipelineCache(Instance &instance,
                             RenderPass &renderPass,
                             const PipelineCacheConfiguration &config)
        : mInstance(instance),
          mRenderPass(renderPass),
          mConfig(config) {
    log::d("Creating PipelineCache");
}

Pipeline &PipelineCache::get(const dn::vulkan::PipelineConfiguration &config) {
    // TODO find a more elegant algorithm
    const std::string key = config.vertexShader + config.fragmentShader + std::to_string(config.wireFrameMode);
    std::lock_guard<std::mutex> guard{mCreatePipelineMutex};

    if (!mPipelines.contains(key)) {
        log::d("Creating Pipeline for VS", config.vertexShader, "and FS", config.fragmentShader);
        mPipelines.emplace(key,
                           Pipeline{mInstance,
                                    mRenderPass,
                                    config
                           }
        );
    }

    return mPipelines.at(key);
}

PipelineCache::~PipelineCache() {
    log::d("Destroying PipelineCache");
}