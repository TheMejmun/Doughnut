//
// Created by Sam on 2024-01-23.
//

#include "graphics/render_api.h"
#include "io/logger.h"

#include <stdexcept>
#include <vector>

using namespace Doughnut;

// Constants
#ifdef NDEBUG
#define ENABLE_VALIDATION_LAYERS false
#else
#define ENABLE_VALIDATION_LAYERS true
#endif
const std::vector<const char *> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation"
};

VulkanAPI::VulkanAPI(const std::string &title) {
    Log::d("Creating VulkanAPI");
    createInstance(title);
}

VulkanAPI::~VulkanAPI() {
    Log::d("Destroying VulkanAPI");
    mInstance.destroy();
}

bool checkValidationLayerSupport() {
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    for (const char *layerName: VALIDATION_LAYERS) {
        bool layerFound = false;

        for (const auto &layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void VulkanAPI::createInstance(const std::string &title) {
    // App Info
    vk::ApplicationInfo applicationInfo(
            title.c_str(), VK_MAKE_VERSION(1, 0, 0),
            nullptr, VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_2
    );

    std::vector<const char *> requiredExtensions{};

    // GLFW extensions
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        requiredExtensions.emplace_back(glfwExtensions[i]);
    }

    // MacOS compatibility
    requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    // Info on which extensions and features we need
    vk::InstanceCreateInfo instanceCreateInfo(
            {vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR},
            &applicationInfo,
            0, {},
            requiredExtensions.size(), requiredExtensions.data()
    );

    // Validation layers
    if (ENABLE_VALIDATION_LAYERS) {
        if (!checkValidationLayerSupport())throw std::runtime_error("Validation layers not available!");

        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        instanceCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    // Done
    mInstance = vk::createInstance(instanceCreateInfo);
}