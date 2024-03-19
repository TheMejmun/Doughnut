//
// Created by Sam on 2024-02-11.
//

#include "graphics/vulkan/shader_module.h"
#include "io/logger.h"
#include "util/importer.h"

using namespace dn;
using namespace dn::vulkan;

ShaderModule::ShaderModule(Context &context, const std::string &filePath)
        : mContext(context) {
    log::v("Creating ShaderModule");
    const auto fileBytes = dn::readFile(filePath);

    vk::ShaderModuleCreateInfo createInfo{
            {},
            // Cast the pointer. Vectors already handle proper memory alignment.
            fileBytes.size(), reinterpret_cast<const uint32_t *>(fileBytes.data())
    };

    mShaderModule = mContext.mDevice.createShaderModule(createInfo);
}

ShaderModule::ShaderModule(dn::vulkan::ShaderModule &&other) noexcept
        : mContext(other.mContext), mShaderModule(std::exchange(other.mShaderModule, nullptr)) {
    log::v("Moving ShaderModule");
}

ShaderModule::~ShaderModule() {
    log::v("Destroying ShaderModule");
    if (mShaderModule != nullptr) { mContext.mDevice.destroy(mShaderModule); }
}