cmake_minimum_required(VERSION 3.24)
project(Doughnut)

set(CMAKE_CXX_STANDARD 20)

# ----------------------------------- SETTINGS -----------------------------------

# TODO check if VK_FOLDER is even necessary or if it is unused.

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

if (NOT DEFINED ENV{VULKAN_SDK})
    message(STATUS "Setting ENV{VULKAN_SDK} from VK_FOLDER")
    set(ENV{VULKAN_SDK} VK_FOLDER)
endif ()

message(STATUS "VK_FOLDER: ${VK_FOLDER}")
message(STATUS "VULKAN_SDK: $ENV{VULKAN_SDK}")

# ----------------------------------- ADD LIBRARY -----------------------------------

# Add files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp)
message(STATUS "SOURCE_FILES: ${SOURCE_FILES}")

# Main library
add_library(Doughnut STATIC source/main.cpp ${HEADER_FILES} ${SOURCE_FILES})

# Add include
target_include_directories(Doughnut PUBLIC include)

# ----------------------------------- ADD DEPENDENCIES -----------------------------------

find_package(assimp CONFIG REQUIRED)
target_link_libraries(Doughnut PRIVATE assimp::assimp)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(Doughnut PRIVATE glfw)
find_package(imgui CONFIG REQUIRED)
target_link_libraries(Doughnut PRIVATE imgui::imgui)
find_package(Stb REQUIRED)
target_include_directories(Doughnut PRIVATE ${Stb_INCLUDE_DIR})
find_package(VulkanHeaders CONFIG)
target_link_libraries(Doughnut PRIVATE Vulkan::Headers)
find_package(glm CONFIG REQUIRED)
target_link_libraries(Doughnut PUBLIC glm::glm)

# output
get_target_property(LL Doughnut LINK_LIBRARIES)
message(STATUS "Linked libraries: ${LL}")

# ----------------------------------- ADD RESOURCES -----------------------------------

# add resources
add_custom_target(CopyAssets COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/CopyAssets.cmake)
add_dependencies(Doughnut CopyAssets)