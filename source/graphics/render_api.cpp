//
// Created by Sam on 2024-01-23.
//

#include "graphics/render_api.h"
#include "io/logger.h"
#include "util/require.h"

#include <stdexcept>
#include <vector>
#include <set>

using namespace dn;

// Constants
const std::vector<const char *> REQUIRED_DEVICE_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};
const std::string PORTABILITY_EXTENSION = "VK_KHR_portability_subset";
#ifdef NDEBUG
#define ENABLE_VALIDATION_LAYERS false
#else
#define ENABLE_VALIDATION_LAYERS true
#endif
const std::vector<const char *> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation"
};

VulkanAPI::VulkanAPI(Window *window, const std::string &title) : mWindow(window) {
   log::d("Creating VulkanAPI");
    createInstance(title);
    createDevice();
}

VulkanAPI::~VulkanAPI() {
   log::d("Destroying VulkanAPI");
    destroyDevice();
    destroyInstance();
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
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
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
        if (!checkValidationLayerSupport())
            throw std::runtime_error("Validation layers not available!");

        instanceCreateInfo.enabledLayerCount = VALIDATION_LAYERS.size();
        instanceCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    mInstance = vk::createInstance(instanceCreateInfo);

    require(
            glfwCreateWindowSurface(mInstance, mWindow->glfwWindow, nullptr, reinterpret_cast<VkSurfaceKHR *>(&mSurface)),
            "Failed to create window surface!"
    );
}

QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface) {
    QueueFamilyIndices indices;
    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

    int i = 0;
    for (const vk::QueueFamilyProperties &queueFamily: queueFamilies) {
        vk::Bool32 presentSupport = device.getSurfaceSupportKHR(i, surface);

        // Look for transfer queue that is not a graphics queue
        if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer &&
            (!(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) || !indices.transferFamily.has_value())) {
            indices.transferFamily = i;
        }

        // Better performance if a queue supports all features together
        // Do not execute if a unified family has already been found
        if (!indices.isUnifiedGraphicsPresentQueue()) {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
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

bool checkExtensionSupport(const vk::PhysicalDevice &device) {
    std::vector<vk::ExtensionProperties> extensionProperties = device.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(REQUIRED_DEVICE_EXTENSIONS.begin(),
                                             REQUIRED_DEVICE_EXTENSIONS.end());

    for (const vk::ExtensionProperties &extension: extensionProperties) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

struct SwapchainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities{};
    std::vector<vk::SurfaceFormatKHR> formats{};
    std::vector<vk::PresentModeKHR> presentModes{};
};

SwapchainSupportDetails querySwapchainSupport(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface) {
    SwapchainSupportDetails details;

    details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.formats = device.getSurfaceFormatsKHR(surface);
    details.presentModes = device.getSurfacePresentModesKHR(surface);

    return details;
}

bool isPhysicalDeviceSuitable(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface, bool strictMode) {
    vk::PhysicalDeviceProperties properties = device.getProperties();
    bool suitable = true;

    // Is discrete GPU
    suitable &= !strictMode || properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;

    // Supports required queues
    QueueFamilyIndices indices = findQueueFamilies(device, surface);
    suitable &= indices.isComplete();

    // Supports required extensions
    suitable &= checkExtensionSupport(device);

    // Supports required swapchain features
    auto swapchainSupport = querySwapchainSupport(device, surface);
    bool swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
    suitable &= swapchainAdequate;

    return suitable;
}

bool checkPortabilityMode(const vk::PhysicalDevice &device) {
    std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

    return std::ranges::any_of(availableExtensions, [](const vk::ExtensionProperties &extension) {
        return PORTABILITY_EXTENSION == extension.extensionName;
    });
}

void VulkanAPI::createDevice() {
    // PHYSICAL
    std::vector<vk::PhysicalDevice> availableDevices = mInstance.enumeratePhysicalDevices();
    require(!availableDevices.empty(), "Failed to find GPUs with  support!");

    std::stringstream stream{};
    stream << "Available physical devices:\n";
    for (const vk::PhysicalDevice &device: availableDevices) {
        stream << "\t" << device.getProperties().deviceName << "\n";
    }
   log::d(stream.str());

    for (const auto &device: availableDevices) {
        if (isPhysicalDeviceSuitable(device, mSurface, true)) {
            mPhysicalDevice = device;
            break;
        } else if (isPhysicalDeviceSuitable(device, mSurface, false)) {
            mPhysicalDevice = device;
            // Keep looking for a better one
        }
    }

    require(mPhysicalDevice, "Failed to find a suitable GPU!");
   log::i("Picked physical device: ", mPhysicalDevice.getProperties().deviceName);

    vk::PhysicalDeviceProperties properties = mPhysicalDevice.getProperties();

    mQueueFamilyIndices = findQueueFamilies(mPhysicalDevice, mSurface);
    mQueueFamilyIndices.print();

    vk::PhysicalDeviceFeatures features = mPhysicalDevice.getFeatures();
    mOptionalFeatures.supportsWireframeMode = features.fillModeNonSolid;

    // LOGICAL

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    // If the indices are the same, the set will merge them -> Only one single queue creation.
    std::set<uint32_t> uniqueQueueFamilies = {mQueueFamilyIndices.graphicsFamily.value(), mQueueFamilyIndices.presentFamily.value(),
                                              mQueueFamilyIndices.transferFamily.value()};

    // TODO more flexible queue creation
    float queuePriority = 1.0f;
    for (uint32_t queueFamily: uniqueQueueFamilies) {
        queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority);
    }

    // Define the features we will use as queried in isPhysicalDeviceSuitable
    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.fillModeNonSolid = mOptionalFeatures.supportsWireframeMode;

    std::vector<const char *> requiredExtensions = REQUIRED_DEVICE_EXTENSIONS;
    if (checkPortabilityMode(mPhysicalDevice)) {
        requiredExtensions.push_back(PORTABILITY_EXTENSION.c_str());
    }

    vk::DeviceCreateInfo createInfo(
            {},
            queueCreateInfos.size(), queueCreateInfos.data(),
            0, {},
            requiredExtensions.size(), requiredExtensions.data(),
            &deviceFeatures
    );

    if (ENABLE_VALIDATION_LAYERS) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    mDevice = mPhysicalDevice.createDevice(createInfo);

    // TODO multiple queues per family?
    mGraphicsQueue = mDevice.getQueue(mQueueFamilyIndices.graphicsFamily.value(), 0);
    mPresentQueue = mDevice.getQueue(mQueueFamilyIndices.presentFamily.value(), 0);
}

void VulkanAPI::destroyDevice() {
    mDevice.destroy();
}

void VulkanAPI::destroyInstance() {
    mInstance.destroySurfaceKHR(mSurface);
    mInstance.destroy();
}