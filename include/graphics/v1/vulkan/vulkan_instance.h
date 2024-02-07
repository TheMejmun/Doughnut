//
// Created by Saman on 24.08.23.
//

#ifndef DOUGHNUT_VULKAN_INSTANCE_H
#define DOUGHNUT_VULKAN_INSTANCE_H

#include "preprocessor.h"

#include <string>
#include <vulkan/vulkan.h>

namespace dn::vulkan::Instance {
    extern VkInstance instance;

    void create(const std::string &title);

    void destroy();

    void printAvailableExtensions();
}

#endif //DOUGHNUT_VULKAN_INSTANCE_H
