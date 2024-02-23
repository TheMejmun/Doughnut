//
// Created by Sam on 2024-02-21.
//

#ifndef DOUGHNUTSANDBOX_PIPELINE_CACHE_H
#define DOUGHNUTSANDBOX_PIPELINE_CACHE_H

#include "pipeline.h"

namespace dn::vulkan {
    struct PipelineCacheConfiguration{
        uint32_t maxFramesInFlight; // TODO is this needed? The 'get' function already contains this info
    };

    class PipelineCache {
    public:
        PipelineCache(Instance &instance,
                      RenderPass &renderPass,
                      const PipelineCacheConfiguration& config);

        ~PipelineCache();

        Pipeline& get(const PipelineConfiguration &config);

    private:
        Instance &mInstance;
        RenderPass &mRenderPass;
        PipelineCacheConfiguration mConfig;

        std::unordered_map<std::string, Pipeline> mPipelines{};
        std::mutex mCreatePipelineMutex{};
    };
}

#endif //DOUGHNUTSANDBOX_PIPELINE_CACHE_H
