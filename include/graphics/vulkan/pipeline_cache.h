//
// Created by Sam on 2024-02-21.
//

#ifndef DOUGHNUTSANDBOX_PIPELINE_CACHE_H
#define DOUGHNUTSANDBOX_PIPELINE_CACHE_H

#include "pipeline.h"

namespace dn::vulkan {
    class PipelineCache {
    public:
        PipelineCache(Instance &instance,
                      RenderPass &renderPass,
                      Buffer &uboBuffer);

        ~PipelineCache();

        Pipeline& get(const PipelineConfiguration &config);

    private:
        Instance &mInstance;
        RenderPass &mRenderPass;
        Buffer &mUboBuffer;

        std::unordered_map<std::string, Pipeline> mPipelines{};
        std::mutex mCreatePipelineMutex{};
    };
}

#endif //DOUGHNUTSANDBOX_PIPELINE_CACHE_H
