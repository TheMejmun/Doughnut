# Vulkan

# ---------------------------------------- UNUSED CODE ----------------------------------------

function(normalize paths)
    string(REPLACE "\\" "/" paths ${paths})
endfunction()

# Vulkan
if (WIN32)
    message(STATUS "Windows mode")
    set(VULKAN_BASE_DIR "C:/VulkanSDK")
elseif (LINUX)
    message(STATUS "Linux mode")
    set(VULKAN_BASE_DIR "$ENV{HOME}/VulkanSDK")
elseif (UNIX)
    message(STATUS "Unix mode")
    set(VULKAN_BASE_DIR "$ENV{HOME}/VulkanSDK")
endif ()

if (NOT DEFINED VK_FOLDER)
    if (DEFINED ENV{VULKAN_SDK})
        message(STATUS "Setting VK_FOLDER from ENV{VULKAN_SDK}")
        set(VK_FOLDER $ENV{VULKAN_SDK})

    else ()
        file(GLOB version_dirs "${VULKAN_BASE_DIR}/*")
        foreach (version_dir ${version_dirs})
            if (IS_DIRECTORY ${version_dir})
                set(VK_FOLDER ${version_dir})
            endif ()
        endforeach ()

        if (UNIX)
            set(VK_FOLDER "${VK_FOLDER}/macOS")
        endif ()
    endif ()
endif ()

string(REPLACE "\\" "/" VK_FOLDER ${VK_FOLDER})

message(STATUS "VK_FOLDER: ${VK_FOLDER}")
set(VK_FOLDER ${VK_FOLDER} PARENT_SCOPE)

if (WIN32)
    set(VK_INCLUDE ${VK_FOLDER}/Include)
    set(VK_LIB ${VK_FOLDER}/Lib)
elseif (LINUX)
    set(VK_INCLUDE ${VK_FOLDER}/include)
    set(VK_LIB ${VK_FOLDER}/lib)
elseif (UNIX)
    set(VK_INCLUDE ${VK_FOLDER}/include)
    set(VK_LIB ${VK_FOLDER}/lib)
endif ()

set(VK_INCLUDE ${VK_INCLUDE} PARENT_SCOPE)
set(VK_LIB ${VK_LIB} PARENT_SCOPE)

# libs
#if (WIN32)
#    # TODO for some reason using the standard SDL2.lib does not work for release builds in Windows
#    file(GLOB SDL_LIB CONFIGURE_DEPENDS "${VK_FOLDER}/Lib/SDL2.lib")
#elseif (LINUX)
#    file(GLOB SDL_LIB CONFIGURE_DEPENDS "${VK_FOLDER}/lib/libSDL2.a")
#elseif (UNIX)
#    file(GLOB SDL_LIB CONFIGURE_DEPENDS "${VK_FOLDER}/lib/libSDL2.a")
#endif ()

#if (WIN32)
#    file(GLOB VK_LIB CONFIGURE_DEPENDS "${VK_FOLDER}/Lib/vulkan-1.lib")
#elseif (LINUX)
#elseif (UNIX)
#endif ()

#set(VK_LIB ${SDL_LIB} ${VK_LIB} PARENT_SCOPE)

#message(STATUS "Before norm: ${VULKAN_LIB_SOURCE}")
#normalize(VULKAN_LIB_SOURCE)
#message(STATUS "After norm: ${VULKAN_LIB_SOURCE}")

#file(COPY ${VULKAN_LIB_SOURCE} DESTINATION "${CMAKE_CURRENT_LIST_DIR}/vk_sdk/lib")
#
#file(GLOB VULKAN_INCLUDES CONFIGURE_DEPENDS "${VK_FOLDER}/Include/*")
#file(COPY ${VULKAN_INCLUDES} DESTINATION "${CMAKE_CURRENT_LIST_DIR}/vk_sdk/include")

# imgui
set(IMGUI_SOURCES
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_vulkan.cpp
        imgui/backends/imgui_impl_sdl2.cpp
)
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC imgui)

target_include_directories(imgui PRIVATE ${VK_INCLUDE})

find_package(SDL2 CONFIG REQUIRED PATHS ${VK_FOLDER}/cmake/SDL2)
target_link_libraries(imgui PRIVATE ${SDL2_LIBRARIES})

include(../util.cmake)
print_target_properties(imgui)