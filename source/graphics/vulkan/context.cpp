//
// Created by Sam on 2024-02-10.
//

#include "graphics/vulkan/context.h"
#include "util/require.h"
#include "graphics/vulkan/swapchain.h"
#include "io/logger.h"

#include <sstream>
#include <set>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

using namespace dn;
using namespace dn::vulkan;

// Constants
const std::array<const char *, 1> REQUIRED_DEVICE_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

const std::string PORTABILITY_EXTENSION = "VK_KHR_portability_subset";

const std::string VALIDATION_EXTENSION = "VK_EXT_debug_utils";

const std::array<const char *, 1> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation"
};

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

// Signature for the vulkan api to call
//static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
//        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//        vk::DebugUtilsMessageTypeFlagsEXT messageType,
//        const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
//        void *pUserData) {
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData) {
    const char *severityString;
    switch ((size_t) messageSeverity) {
        case (size_t) vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose: {
            severityString = "verbose";
            break;
        }
        case (size_t) vk::DebugUtilsMessageSeverityFlagBitsEXT::eError: {
            severityString = "error";
            break;
        }
        case (size_t) vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning: {
            severityString = "warning";
            break;
        }
        case (size_t) vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo: {
            severityString = "info";
            break;
        }
    }

    std::stringstream objectStream{};
    for (size_t i = 0; i < pCallbackData->objectCount; ++i) {
        void *ptr = (void *) pCallbackData->pObjects[i].objectHandle;
        if (debugInfos.contains(ptr))
            objectStream << "\n\t" << debugInfos.at(ptr);
    }

    if ((size_t) messageSeverity >= (size_t) vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        log::e("vk", severityString, pCallbackData->messageIdNumber,"\b:", pCallbackData->pMessage, "\nObjects:", objectStream.str());
    } else {
        log::d("vk", severityString, pCallbackData->messageIdNumber, "\b:", pCallbackData->pMessage, "\nObjects:", objectStream.str());
    }

    return vk::False;
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

// https://github.com/KhronosGroup/Vulkan-Hpp/issues/1717
PFN_vkCreateDebugUtilsMessengerEXT getDebugMessengerCreateFn(vk::Instance instance) {
    auto createFnPtr = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (!createFnPtr) {
        throw std::runtime_error("Unable to find pfnVkCreateDebugUtilsMessengerEXT function.");
    }
    return createFnPtr;
}

PFN_vkDestroyDebugUtilsMessengerEXT getDebugMessengerDestroyFn(vk::Instance instance) {
    auto destroyFnPtr = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (!destroyFnPtr) {
        throw std::runtime_error("Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.");
    }
    return destroyFnPtr;
}

Context::Context(Window &window, ContextConfiguration config) : mWindow(window) {

    // INSTANCE

    vk::ApplicationInfo applicationInfo(
            mWindow.mTitle.c_str(),
            vk::makeApiVersion(0, 1, 0, 0),
            nullptr,
            vk::makeApiVersion(0, 1, 0, 0),
            vk::ApiVersion12
    );

    std::vector<const char *> requiredInstanceExtensions{};

    // SDL extensions
    unsigned int sdlExtensionCount;
    std::vector<const char *> sdlExtensionNames{};
    require(
            SDL_Vulkan_GetInstanceExtensions((SDL_Window *) mWindow.mHandle, &sdlExtensionCount, nullptr) == SDL_TRUE,
            "Error getting SDL Vulkan extensions"
    );
    sdlExtensionNames.resize(sdlExtensionCount);
    require(
            SDL_Vulkan_GetInstanceExtensions((SDL_Window *) mWindow.mHandle, &sdlExtensionCount,
                                             sdlExtensionNames.data()) == SDL_TRUE,
            "Error getting SDL Vulkan extensions"
    );

    for (uint32_t i = 0; i < sdlExtensionCount; i++) {
        requiredInstanceExtensions.emplace_back(sdlExtensionNames[i]);
    }

    // MacOS compatibility
    requiredInstanceExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    // Validation layers
#ifdef ENABLE_VALIDATION_LAYERS
    if (!checkValidationLayerSupport())
        throw std::runtime_error("Validation layers not available!");

    requiredInstanceExtensions.emplace_back(VALIDATION_EXTENSION.c_str());

    // Info on which extensions and features we need
    vk::InstanceCreateInfo instanceCreateInfo(
            {vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR},
            &applicationInfo,
            VALIDATION_LAYERS.size(), VALIDATION_LAYERS.data(),
            requiredInstanceExtensions.size(), requiredInstanceExtensions.data()
    );
#else
    // Info on which extensions and features we need
    vk::InstanceCreateInfo instanceCreateInfo(
            {vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR},
            &applicationInfo,
            0, {},
            requiredInstanceExtensions.size(), requiredInstanceExtensions.data()
    );
#endif

    mInstance = vk::createInstance(instanceCreateInfo);

    require(
            SDL_Vulkan_CreateSurface((SDL_Window *) mWindow.mHandle, mInstance,
                                     reinterpret_cast<VkSurfaceKHR *>(&mSurface)) == SDL_TRUE,
            "Failed to create window surface!"
    );

    // PHYSICAL

    std::vector<vk::PhysicalDevice> availableDevices = mInstance.enumeratePhysicalDevices();
    require(!availableDevices.empty(), "Failed to find GPUs with  support!");

    std::stringstream stream{};
    stream << "Available physical devices:";
    for (const vk::PhysicalDevice &device: availableDevices) {
        stream << "\n\t" << device.getProperties().deviceName;
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
    mOptionalFeatures.supportsAnisotropicFiltering = features.samplerAnisotropy;

    // LOGICAL

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    // If the indices are the same, the set will merge them -> Only one single queue creation.
    std::set<uint32_t> uniqueQueueFamilies = {*mQueueFamilyIndices.graphicsFamily,
                                              *mQueueFamilyIndices.presentFamily,
                                              *mQueueFamilyIndices.transferFamily};

    // TODO more flexible queue creation
    float queuePriority = 1.0f;
    for (uint32_t queueFamily: uniqueQueueFamilies) {
        queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority);
    }

    // Define the features we will use as queried in isPhysicalDeviceSuitable
    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.fillModeNonSolid = mOptionalFeatures.supportsWireframeMode;
    deviceFeatures.samplerAnisotropy = mOptionalFeatures.supportsAnisotropicFiltering;

    std::vector<const char *> requiredDeviceExtensions{};
    requiredDeviceExtensions.insert(requiredDeviceExtensions.end(), REQUIRED_DEVICE_EXTENSIONS.begin(),
                                    REQUIRED_DEVICE_EXTENSIONS.end());
    if (checkPortabilityMode(mPhysicalDevice)) {
        requiredDeviceExtensions.push_back(PORTABILITY_EXTENSION.c_str());
    }

    vk::DeviceCreateInfo createInfo(
            {},
            queueCreateInfos.size(), queueCreateInfos.data(),
            0, {},
            requiredDeviceExtensions.size(), requiredDeviceExtensions.data(),
            &deviceFeatures
    );

#ifdef ENABLE_VALIDATION_LAYERS
    createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#endif

    mDevice = mPhysicalDevice.createDevice(createInfo);

    // TODO multiple queues per family?
    mGraphicsQueue = mDevice.getQueue(mQueueFamilyIndices.graphicsFamily.value(), 0);
    mPresentQueue = mDevice.getQueue(mQueueFamilyIndices.presentFamily.value(), 0);
    mTransferQueue = mDevice.getQueue(mQueueFamilyIndices.transferFamily.value(), 0);

#ifdef ENABLE_VALIDATION_LAYERS
    auto createFun = getDebugMessengerCreateFn(mInstance);
//    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo(
//            vk::DebugUtilsMessengerCreateFlagsEXT{},
//            {vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
//             vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
//             vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
//             vk::DebugUtilsMessageSeverityFlagBitsEXT::eError},
//            {vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
//             vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance},
//            debugCallback
//    );
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
    debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerCreateInfo.pfnUserCallback = debugCallback;
    VkDebugUtilsMessengerEXT messenger;
    auto result = createFun(mInstance, &debugMessengerCreateInfo, nullptr, &messenger);
    require(result, "Failed to create debug messenger!");
    mDebugMessenger = messenger;
#endif

    log::d("Created Context");
}

void Context::awaitIdle(bool graphicsQueue, bool presentQueue, bool transferQueue) const {
    if (graphicsQueue && mGraphicsQueue != nullptr) mGraphicsQueue.waitIdle();
    if (presentQueue && mPresentQueue != nullptr) mPresentQueue.waitIdle();
    if (transferQueue && mTransferQueue != nullptr) mTransferQueue.waitIdle();
}

Context::~Context() {
    log::d("Destroying Context");
#ifdef ENABLE_VALIDATION_LAYERS
    auto destroy = getDebugMessengerDestroyFn(mInstance);

    if (mDebugMessenger != nullptr) { destroy(mInstance, mDebugMessenger, nullptr); }

#endif
    if (mDevice != nullptr) { mDevice.destroy(); }
    if (mSurface != nullptr) { mInstance.destroySurfaceKHR(mSurface); }
    if (mInstance != nullptr) { mInstance.destroy(); }
}