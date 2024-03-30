//
// Created by Sam on 2024-02-11.
//

#include "graphics/vulkan/shader_module.h"
#include "io/logger.h"
#include "util/importer.h"

using namespace dn;
using namespace dn::vulkan;

ShaderModule::ShaderModule(Context &context, const ShaderModuleConfiguration &config)
        : Handle<vk::ShaderModule, ShaderModuleConfiguration>(context, config) {

    Shader shaderCode{config.filePath,config.type};
    shaderCode.compile();

    vk::ShaderModuleCreateInfo createInfo{
            {},
            // Cast the pointer. Vectors already handle proper memory alignment.
            shaderCode.size(), shaderCode.mBinary.data()
    };

    mVulkan = mContext.mDevice.createShaderModule(createInfo);
}

ShaderModule::~ShaderModule() {
    if (mVulkan != nullptr) { mContext.mDevice.destroy(mVulkan); }
}