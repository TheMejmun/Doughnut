//
// Created by Sam on 2024-02-22.
//

#ifndef DOUGHNUTSANDBOX_SAMPLER_H
#define DOUGHNUTSANDBOX_SAMPLER_H

#include "image.h"
#include "handle.h"

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

class Sampler :public Handle<vk::Sampler, SamplerConfiguration>{
    public:
        Sampler(Context &context,
                const SamplerConfiguration &config);

        Sampler(Sampler &&other) = default;

        ~Sampler();
    };
}

#endif //DOUGHNUTSANDBOX_SAMPLER_H
