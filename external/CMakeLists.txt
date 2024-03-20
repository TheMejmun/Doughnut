# Vulkan

# ---------------------------------------- UNUSED CODE ----------------------------------------

function(normalize paths)
    string(REPLACE "\\" "/" paths ${paths})
endfunction()

# Vulkan

# includes
if (WIN32)
    set(VK_INCLUDE ${VK_FOLDER}/Include PARENT_SCOPE)
elseif (LINUX)
    set(VK_INCLUDE ${VK_FOLDER}/include PARENT_SCOPE)
elseif (UNIX)
    set(VK_INCLUDE ${VK_FOLDER}/include PARENT_SCOPE)
endif ()

# libs
if (WIN32)
    file(GLOB SDL_LIB CONFIGURE_DEPENDS "${VK_FOLDER}/Lib/SDL2.lib")
elseif (LINUX)
    file(GLOB SDL_LIB CONFIGURE_DEPENDS "${VK_FOLDER}/lib/libSDL2.a")
elseif (UNIX)
    file(GLOB SDL_LIB CONFIGURE_DEPENDS "${VK_FOLDER}/lib/libSDL2.a")
endif ()

set(VK_LIB ${SDL_LIB} PARENT_SCOPE)

#message(STATUS "Before norm: ${VULKAN_LIB_SOURCE}")
#normalize(VULKAN_LIB_SOURCE)
#message(STATUS "After norm: ${VULKAN_LIB_SOURCE}")

#file(COPY ${VULKAN_LIB_SOURCE} DESTINATION "${CMAKE_CURRENT_LIST_DIR}/vk_sdk/lib")
#
#file(GLOB VULKAN_INCLUDES CONFIGURE_DEPENDS "${VK_FOLDER}/Include/*")
#file(COPY ${VULKAN_INCLUDES} DESTINATION "${CMAKE_CURRENT_LIST_DIR}/vk_sdk/include")