cmake_minimum_required(VERSION 3.24)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
project(Doughnut)

set(CMAKE_CXX_STANDARD 20)

add_link_options("-v")
set(CMAKE_VERBOSE_MAKEFILE ON)

if (WIN32)
    message(STATUS "Windows mode")
    set(VK_FOLDER "C:/VulkanSDK/1.3.239.0")
    message(STATUS "VULKAN_SDK: $ENV{VULKAN_SDK}")
elseif (LINUX)
    message(STATUS "Linux mode")
    set(VK_FOLDER "$ENV{HOME}/VulkanSDK/1.3.239.0")
    message(STATUS "VULKAN_SDK: $ENV{VULKAN_SDK}")
elseif (UNIX)
    message(STATUS "Unix mode")
    set(VK_FOLDER "$ENV{HOME}/VulkanSDK/1.3.239.0/macOS")
    message(STATUS "VULKAN_SDK: $ENV{VULKAN_SDK}")
endif ()

set(HEADER_FOLDER include)
set(SOURCE_FOLDER source)
set(EXTERNAL_FOLDER external)
set(LIB_FOLDER lib)

# Add files
# Add any added source/header files here
set(HEADER_FILES
        ${HEADER_FOLDER}/application.h
        ${HEADER_FOLDER}/preprocessor.h
        ${HEADER_FOLDER}/typedefs.h

        ${HEADER_FOLDER}/graphics/v1/ui.h
        ${HEADER_FOLDER}/graphics/v1/renderer.h
        ${HEADER_FOLDER}/graphics/colors.h
        ${HEADER_FOLDER}/graphics/vertex.h
        ${HEADER_FOLDER}/graphics/v1/uniform_buffer_object.h
        ${HEADER_FOLDER}/graphics/triangle.h
        ${HEADER_FOLDER}/graphics/v1/render_state.h

        ${HEADER_FOLDER}/graphics/v1/vulkan/vulkan_images.h
        ${HEADER_FOLDER}/graphics/v1/vulkan/vulkan_memory.h
        ${HEADER_FOLDER}/graphics/v1/vulkan/vulkan_buffers.h
        ${HEADER_FOLDER}/graphics/v1/vulkan/vulkan_devices.h
        ${HEADER_FOLDER}/graphics/v1/vulkan/vulkan_instance.h
        ${HEADER_FOLDER}/graphics/v1/vulkan/vulkan_validation.h
        ${HEADER_FOLDER}/graphics/v1/vulkan/vulkan_swapchain.h
        ${HEADER_FOLDER}/graphics/v1/vulkan/vulkan_renderpasses.h
        ${HEADER_FOLDER}/graphics/v1/vulkan/vulkan_imgui.h
        ${HEADER_FOLDER}/graphics/v1/vulkan/vulkan_state.h
        ${HEADER_FOLDER}/graphics/v1/vulkan/pnext_chain_reader.h

        ${HEADER_FOLDER}/io/input_manager.h
        ${HEADER_FOLDER}/io/window_manager.h

        ${HEADER_FOLDER}/ecs/entity_manager.h
        ${HEADER_FOLDER}/ecs/system_manager.h
        ${HEADER_FOLDER}/ecs/entity_system_manager.h
        ${HEADER_FOLDER}/ecs/entities/dense_sphere.h
        ${HEADER_FOLDER}/ecs/entities/camera.h
        ${HEADER_FOLDER}/ecs/entities/input_state_entity.h
        ${HEADER_FOLDER}/ecs/entities/monkey.h
        ${HEADER_FOLDER}/ecs/components/render_mesh.h
        ${HEADER_FOLDER}/ecs/components/render_mesh_simplifiable.h
        ${HEADER_FOLDER}/ecs/components/input_state.h
        ${HEADER_FOLDER}/ecs/components/rotating_sphere.h
        ${HEADER_FOLDER}/ecs/components/projector.h
        ${HEADER_FOLDER}/ecs/components/ui_state.h
        ${HEADER_FOLDER}/ecs/systems/camera_controller.h
        ${HEADER_FOLDER}/ecs/systems/sphere_controller.h
        ${HEADER_FOLDER}/ecs/systems/mesh_simplifier_controller.h

        ${HEADER_FOLDER}/physics/transformer.h

        ${HEADER_FOLDER}/util/timer.h
        ${HEADER_FOLDER}/util/importer.h
        ${HEADER_FOLDER}/util/byte_size.h
        ${HEADER_FOLDER}/util/performance_logging.h


        ${HEADER_FOLDER}/ecs/entities/ui_state_entity.h
        ${HEADER_FOLDER}/util/os.h
        ${HEADER_FOLDER}/core/scheduler.h
        ${HEADER_FOLDER}/io/logger.h
        ${HEADER_FOLDER}/util/templates.h
        ${HEADER_FOLDER}/core/types.h
        ${HEADER_FOLDER}/graphics/texture.h
        ${HEADER_FOLDER}/io/resource_pool.h
        ${HEADER_FOLDER}/graphics/render_api.h
        ${HEADER_FOLDER}/graphics/renderer_v2.h
)
message(STATUS "HEADER_FILES: ${HEADER_FILES}")
set(SOURCE_FILES
        ${SOURCE_FOLDER}/application.cpp

        ${SOURCE_FOLDER}/graphics/v1/ui.cpp
        ${SOURCE_FOLDER}/graphics/v1/renderer/drawing.cpp
        ${SOURCE_FOLDER}/graphics/v1/renderer/common.cpp
        ${SOURCE_FOLDER}/graphics/v1/renderer/shaders.cpp
        ${SOURCE_FOLDER}/graphics/v1/renderer/systems.cpp
        ${SOURCE_FOLDER}/graphics/v1/renderer/ui.cpp
        ${SOURCE_FOLDER}/graphics/colors.cpp

        ${SOURCE_FOLDER}/graphics/v1/vulkan/vulkan_buffers.cpp
        ${SOURCE_FOLDER}/graphics/v1/vulkan/vulkan_images.cpp
        ${SOURCE_FOLDER}/graphics/v1/vulkan/vulkan_memory.cpp
        ${SOURCE_FOLDER}/graphics/v1/vulkan/vulkan_devices.cpp
        ${SOURCE_FOLDER}/graphics/v1/vulkan/vulkan_instance.cpp
        ${SOURCE_FOLDER}/graphics/v1/vulkan/vulkan_validation.cpp
        ${SOURCE_FOLDER}/graphics/v1/vulkan/vulkan_swapchain.cpp
        ${SOURCE_FOLDER}/graphics/v1/vulkan/vulkan_renderpasses.cpp
        ${SOURCE_FOLDER}/graphics/v1/vulkan/vulkan_imgui.cpp

        ${SOURCE_FOLDER}/io/input_manager.cpp
        ${SOURCE_FOLDER}/io/window_manager.cpp

        ${SOURCE_FOLDER}/ecs/components/projector.cpp
        ${SOURCE_FOLDER}/ecs/systems/camera_controller.cpp
        ${SOURCE_FOLDER}/ecs/systems/sphere_controller.cpp
        ${SOURCE_FOLDER}/ecs/systems/mesh_simplifier_controller.cpp

        ${SOURCE_FOLDER}/physics/transformer.cpp

        ${SOURCE_FOLDER}/util/importer.cpp
        ${SOURCE_FOLDER}/util/timer.cpp
        ${SOURCE_FOLDER}/util/performance_logging.cpp


        ${SOURCE_FOLDER}/ecs/entity_manager.cpp
        ${SOURCE_FOLDER}/ecs/system_manager.cpp
        ${SOURCE_FOLDER}/ecs/entities/camera.cpp
        ${SOURCE_FOLDER}/ecs/entities/dense_sphere.cpp
        ${SOURCE_FOLDER}/ecs/entities/input_state_entity.cpp
        ${SOURCE_FOLDER}/ecs/entities/monkey.cpp
        ${SOURCE_FOLDER}/ecs/entities/ui_state_entity.cpp
        ${SOURCE_FOLDER}/core/scheduler.cpp
        ${SOURCE_FOLDER}/io/logger.cpp
        ${SOURCE_FOLDER}/graphics/texture.cpp
)
message(STATUS "SOURCE_FILES: ${SOURCE_FILES}")
# Main executable
add_executable(Doughnut ${SOURCE_FOLDER}/main.cpp ${HEADER_FILES} ${SOURCE_FILES})

# Add folders
target_include_directories(Doughnut PUBLIC ${HEADER_FOLDER})
target_include_directories(Doughnut PUBLIC ${SOURCE_FOLDER})

# vcpkg autogen
# glfw
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(Doughnut PRIVATE glfw)
# assimp
find_package(assimp CONFIG REQUIRED)
target_link_libraries(Doughnut PRIVATE assimp::assimp)
# imgui
find_package(imgui CONFIG REQUIRED)
target_link_libraries(Doughnut PRIVATE imgui::imgui)
# stb
find_package(Stb REQUIRED)
target_include_directories(Doughnut PRIVATE ${Stb_INCLUDE_DIR})
# vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(Doughnut PRIVATE Vulkan::Vulkan)

# output
get_target_property(LL Doughnut LINK_LIBRARIES)
message(STATUS "Linked libraries: ${LL}")

# add resources
add_custom_target(CopyAssets COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/CopyAssets.cmake)
add_dependencies(Doughnut CopyAssets)