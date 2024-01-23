//
// Created by Saman on 24.08.23.
//

#ifndef DOUGHNUT_VULKAN_VALIDATION_H
#define DOUGHNUT_VULKAN_VALIDATION_H

#include "preprocessor.h"

#include <vector>


namespace Doughnut::Graphics::Vk::Validation {
    extern const bool ENABLE_VALIDATION_LAYERS;
    extern const std::vector<const char *> VALIDATION_LAYERS;

    bool checkValidationLayerSupport();
}

#endif //DOUGHNUT_VULKAN_VALIDATION_H
