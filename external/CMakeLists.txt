# Vulkan

# ---------------------------------------- UNUSED CODE ----------------------------------------

function(normalize paths)
    string(REPLACE "\\" "/" paths ${paths})
endfunction()

if (WIN32)
    file(GLOB VULKAN_LIB_SOURCE CONFIGURE_DEPENDS "${VK_FOLDER}/Lib/*.lib")
elseif (LINUX)
    file(GLOB VULKAN_LIB_SOURCE CONFIGURE_DEPENDS "${VK_FOLDER}/Lib/*.a")
elseif (UNIX)
    file(GLOB VULKAN_LIB_SOURCE CONFIGURE_DEPENDS "${VK_FOLDER}/Lib/*.a")
endif ()

#message(STATUS "Before norm: ${VULKAN_LIB_SOURCE}")
#normalize(VULKAN_LIB_SOURCE)
#message(STATUS "After norm: ${VULKAN_LIB_SOURCE}")

file(COPY ${VULKAN_LIB_SOURCE} DESTINATION "${CMAKE_CURRENT_LIST_DIR}/vk_sdk/lib")

file(GLOB VULKAN_INCLUDES CONFIGURE_DEPENDS "${VK_FOLDER}/Include/*")
file(COPY ${VULKAN_INCLUDES} DESTINATION "${CMAKE_CURRENT_LIST_DIR}/vk_sdk/include")