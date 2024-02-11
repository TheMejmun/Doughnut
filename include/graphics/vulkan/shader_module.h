//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUTSANDBOX_SHADER_MODULE_H
#define DOUGHNUTSANDBOX_SHADER_MODULE_H

#include "graphics/vulkan/instance.h"

#include <string>
#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    class ShaderModule {
    public:
        ShaderModule(Instance &instance, const std::string &filePath);

        ShaderModule(ShaderModule &&other) noexcept;

        ~ShaderModule();

        vk::ShaderModule mShaderModule= nullptr;

    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUTSANDBOX_SHADER_MODULE_H
