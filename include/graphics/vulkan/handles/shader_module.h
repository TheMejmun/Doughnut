//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUT_SHADER_MODULE_H
#define DOUGHNUT_SHADER_MODULE_H

#include "graphics/vulkan/context.h"
#include "graphics/vulkan/handle.h"
#include "graphics/shader.h"

#include <string>
#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct ShaderModuleConfiguration {
        std::string filePath;
        ShaderType type;
    };

    class ShaderModule : public Handle<vk::ShaderModule, ShaderModuleConfiguration> {
    public:
        ShaderModule(Context &context, const ShaderModuleConfiguration &config);

        ShaderModule(ShaderModule &&other) = default;

        ~ShaderModule();
    };
}

#endif //DOUGHNUT_SHADER_MODULE_H
