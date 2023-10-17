//
// Created by Saman on 24.08.23.
//

#include "graphics/vulkan/vulkan_devices.h"
#include "graphics/vulkan/vulkan_validation.h"
#include "graphics/vulkan/vulkan_instance.h"
#include "graphics/vulkan/vulkan_swapchain.h"
#include "io/logger.h"

#include <set>
#include <cstdint>

using namespace Doughnut::GFX::Vk;
using namespace Doughnut;

// Constant
extern const std::vector<const char *> Devices::REQUIRED_DEVICE_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};
extern const std::string Devices::PORTABILITY_EXTENSION = "VK_KHR_portability_subset";

// Global
VkPhysicalDevice Devices::physical = nullptr;
VkDevice Devices::logical = nullptr;
VkQueue Devices::graphicsQueue = nullptr;
VkQueue Devices::presentQueue = nullptr;
Devices::QueueFamilyIndices Devices::queueFamilyIndices{};
Devices::OptionalFeatures  Devices::optionalFeatures{};

void Devices::create() {
    Log::i("Creating Devices");

    Devices::pickPhysical();
    Devices::createLogical();
}

void Devices::printAvailablePhysicalDevices() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(Instance::instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(Instance::instance, &deviceCount, devices.data());

    std::stringstream stream{};
    stream << "Available physical devices:\n";

    for (const auto &device: devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        stream << "\t" << deviceProperties.deviceName << "\n";
    }
    Log::v(stream.str());
}

void Devices::pickPhysical() {
    printAvailablePhysicalDevices();

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(Instance::instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with  support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(Instance::instance, &deviceCount, devices.data());

    for (const auto &device: devices) {
        if (isPhysicalDeviceSuitable(device, true)) {
            Devices::physical = device;
            break;
        } else if (isPhysicalDeviceSuitable(device, false)) {
            Devices::physical = device;
            // Keep looking for a better one
        }
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(Devices::physical, &deviceProperties);
    Log::i("Picked physical device: ", deviceProperties.deviceName);

    if (Devices::physical == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    Devices::queueFamilyIndices = Devices::findQueueFamilies(Devices::physical);
    Devices::queueFamilyIndices.print();

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(Devices::physical, &deviceFeatures);
    Devices::optionalFeatures.supportsWireframeMode = deviceFeatures.fillModeNonSolid;
}

bool Devices::isPhysicalDeviceSuitable(VkPhysicalDevice device, bool strictMode) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Is discrete GPU
    bool suitable = true;
    if (strictMode) {
        suitable = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    }

    // Supports required queues
    QueueFamilyIndices indices = Devices::findQueueFamilies(device);
    suitable = suitable && indices.isComplete();

    // Supports required extensions
    suitable = suitable && checkExtensionSupport(device);

    // Supports required swapchain features
    auto swapchainSupport = Swapchain::querySwapchainSupport(device);
    bool swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
    suitable = suitable && swapchainAdequate;

    return suitable;
}

bool Devices::checkExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(Devices::REQUIRED_DEVICE_EXTENSIONS.begin(),
            Devices::REQUIRED_DEVICE_EXTENSIONS.end());

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    std::stringstream stream{};
    stream << "Available device extensions for " << deviceProperties.deviceName << ":\n";
    for (const auto &extension: availableExtensions) {
        stream << '\t' << extension.extensionName << "\n";
        requiredExtensions.erase(extension.extensionName);
    }
    Log::v(stream.str());

    return requiredExtensions.empty();
}

bool Devices::checkPortabilityMode(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    for (const auto &extension: availableExtensions) {
        if (PORTABILITY_EXTENSION == extension.extensionName) {
            return true;
        }
    }

    return false;
}

Devices::QueueFamilyIndices Devices::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily: queueFamilies) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, Swapchain::surface, &presentSupport);

        // Look for transfer queue that is not a graphics queue
        if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT &&
            (!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) || !indices.transferFamily.has_value())) {
            indices.transferFamily = i;
        }

        // Better performance if a queue supports all features together
        // Do not execute if a unified family has already been found
        if (!indices.isUnifiedGraphicsPresentQueue()) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            if (presentSupport) {
                indices.presentFamily = i;
            }
        }

        ++i;
    }

    return indices;
}

void Devices::createLogical() {
    auto indices = Devices::queueFamilyIndices;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    // If the indices are the same, the set will merge them -> Only one single queue creation.
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(),
                                              indices.transferFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily: uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Define the features we will use as queried in isPhysicalDeviceSuitable
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.fillModeNonSolid = Devices::optionalFeatures.supportsWireframeMode;

    VkDeviceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    std::vector<const char *> requiredExtensions = REQUIRED_DEVICE_EXTENSIONS;
    if (checkPortabilityMode(Devices::physical)) {
        requiredExtensions.push_back(PORTABILITY_EXTENSION.c_str());
    }

    createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (Validation::ENABLE_VALIDATION_LAYERS) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(Validation::VALIDATION_LAYERS.size());
        createInfo.ppEnabledLayerNames = Validation::VALIDATION_LAYERS.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(Devices::physical, &createInfo, nullptr, &Devices::logical) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device!");
    }

    // Get each queue
    vkGetDeviceQueue(Devices::logical, indices.graphicsFamily.value(), 0, &Devices::graphicsQueue);
    vkGetDeviceQueue(Devices::logical, indices.presentFamily.value(), 0, &Devices::presentQueue);
}

bool Devices::QueueFamilyIndices::isComplete() const {
    return this->graphicsFamily.has_value() &&
           this->presentFamily.has_value() &&
           this->transferFamily.has_value();
}

bool Devices::QueueFamilyIndices::isUnifiedGraphicsPresentQueue() const {
    if (!this->graphicsFamily.has_value() ||
        !this->presentFamily.has_value())
        return false;
    return this->graphicsFamily.value() == this->presentFamily.value();
}

bool Devices::QueueFamilyIndices::hasUniqueTransferQueue() const {
    if (!this->graphicsFamily.has_value() ||
        !this->presentFamily.has_value())
        return false;
    return this->graphicsFamily.value() != this->transferFamily.value();
}

void Devices::QueueFamilyIndices::print() {
    Log::v(
            "QueueFamilyIndices: Graphics:", this->graphicsFamily.value(), "Present:", this->presentFamily.value(), "Transfer:", this->transferFamily.value()
    );
}

void Devices::destroy() {
    Log::i("Destroying Devices");

    vkDestroyDevice(Devices::logical, nullptr);
}