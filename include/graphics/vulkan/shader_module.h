//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUT_SHADER_MODULE_H
#define DOUGHNUT_SHADER_MODULE_H

#include "graphics/vulkan/context.h"

#include <string>
#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    class ShaderModule {
    public:
        ShaderModule(Context &context, const std::string &filePath);

        ShaderModule(ShaderModule &&other) noexcept;

        ~ShaderModule();

        vk::ShaderModule mShaderModule= nullptr;

    private:
        Context &mContext;
    };
}

#endif //DOUGHNUT_SHADER_MODULE_H
