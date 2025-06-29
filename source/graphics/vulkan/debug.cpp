////
//// Created by Saman on 29.06.25.
////
//
//#include "graphics/vulkan/debug.h"
//#include "io/logger.h"
//
//using namespace dn;
//using namespace dn::vulkan;
//
//// Signature for the vulkan api to call
//VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
//        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//        vk::DebugUtilsMessageTypeFlagsEXT messageType,
//        const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
//        void *pUserData
//) {
//    const char *severityString;
//    switch (messageSeverity) {
//        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose: {
//            severityString = "verbose";
//            break;
//        }
//        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError: {
//            severityString = "error";
//            break;
//        }
//        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning: {
//            severityString = "warning";
//            break;
//        }
//        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo: {
//            severityString = "info";
//            break;
//        }
//    }
//
//    if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
//        log::e("vk", severityString, ":", pCallbackData->pMessage);
//    } else {
//        log::d("vk", severityString, ":", pCallbackData->pMessage);
//    }
//
//    return vk::False;
//}
