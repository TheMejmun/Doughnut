//
// Created by Sam on 2024-01-23.
//

#ifndef DOUGHNUT_RENDER_API_H
#define DOUGHNUT_RENDER_API_H

#include "preprocessor.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <string>

namespace Doughnut {
    class VulkanAPI {
    public:
        VulkanAPI(const std::string &title);

        ~VulkanAPI();

    private:
        void createInstance(const std::string &title);

        vk::Instance mInstance = nullptr;
    };
}

#endif //DOUGHNUT_RENDER_API_H
