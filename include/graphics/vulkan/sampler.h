//
// Created by Sam on 2024-02-22.
//

#ifndef DOUGHNUTSANDBOX_SAMPLER_H
#define DOUGHNUTSANDBOX_SAMPLER_H

#include "image.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    enum EdgeMode {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP,
        MIRROR_CLAMP,
        SOLID_WHITE,
        SOLID_BLACK
    };

    struct SamplerConfiguration {
        EdgeMode edgeMode;
    };

    class Sampler {
    public:
        Sampler(Instance &instance,
                const SamplerConfiguration &config);

        Sampler(Sampler &&other) = default;

        ~Sampler();

        vk::Sampler mSampler;

    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUTSANDBOX_SAMPLER_H
