//
// Created by Sam on 2024-02-22.
//

#ifndef DOUGHNUT_SAMPLER_H
#define DOUGHNUT_SAMPLER_H

#include "graphics/vulkan/handles/image.h"
#include "graphics/vulkan/handle.h"

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

#endif //DOUGHNUT_SAMPLER_H
