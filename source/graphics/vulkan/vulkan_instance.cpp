//
// Created by Saman on 24.08.23.
//

#include "graphics/vulkan/vulkan_instance.h"
#include "graphics/vulkan/vulkan_validation.h"
#include "io/logger.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>

using namespace Doughnut::GFX::Vk;
using namespace Doughnut;

// Global
VkInstance Instance::instance = nullptr;

void Instance::create(const std::string &title) {
    Log::i("Creating Instance");

    // App Info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = title.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = nullptr;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    // Info on which extensions and features we need
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    std::vector<const char *> requiredExtensions;

    // GLFW extensions
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        requiredExtensions.emplace_back(glfwExtensions[i]);
    }

    // MacOS compatibility
    requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    // Extensions final
    Instance::printAvailableExtensions();
    createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    // Validation layers
    if (Validation::ENABLE_VALIDATION_LAYERS) {
        if (!Validation::checkValidationLayerSupport()) {
            throw std::runtime_error("Validation layers not available!");
        }
        createInfo.enabledLayerCount = static_cast<uint32_t>(Validation::VALIDATION_LAYERS.size());
        createInfo.ppEnabledLayerNames = Validation::VALIDATION_LAYERS.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    // Done
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        std::runtime_error("Failed to create instance!");
    }
}

void Instance::printAvailableExtensions() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::stringstream stream{};
    stream << "Available instance extensions:\n";
    for (const auto &extension: extensions) {
        stream << '\t' << extension.extensionName << "\n";
    }
    Log::v(stream.str());
}

void Instance::destroy() {
    Log::i("Destroying Instance");

    vkDestroyInstance(instance, nullptr);
}