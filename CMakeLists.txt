cmake_minimum_required(VERSION 3.24)
project(Doughnut)

set(CMAKE_CXX_STANDARD 20)

# ----------------------------------- SETTINGS -----------------------------------

if (WIN32)
    message(STATUS "Windows mode")
    #    set(VK_FOLDER "C:/VulkanSDK/1.3.239.0")
    message(STATUS "VULKAN_SDK: $ENV{VULKAN_SDK}")

elseif (LINUX)
    message(STATUS "Linux mode")
    #    set(VK_FOLDER "$ENV{HOME}/VulkanSDK/1.3.239.0")
    message(STATUS "VULKAN_SDK: $ENV{VULKAN_SDK}")

elseif (UNIX)
    message(STATUS "Unix mode")
    #    set(VK_FOLDER "$ENV{HOME}/VulkanSDK/1.3.239.0/macOS")
    message(STATUS "VULKAN_SDK: $ENV{VULKAN_SDK}")

endif ()

# ----------------------------------- ADD LIBRARY -----------------------------------

# Add files
# Add any added source/header files here
set(HEADER_FILES
        include/application.h
        include/preprocessor.h
        include/typedefs.h

        include/core/scheduler.h

        include/graphics/colors.h
        include/graphics/vertex.h
        include/graphics/triangle.h

        include/graphics/v1/ui.h
        include/graphics/v1/renderer.h
        include/graphics/v1/uniform_buffer_object.h
        include/graphics/v1/render_state.h

        include/graphics/v1/vulkan/vulkan_images.h
        include/graphics/v1/vulkan/vulkan_memory.h
        include/graphics/v1/vulkan/vulkan_buffers.h
        include/graphics/v1/vulkan/vulkan_devices.h
        include/graphics/v1/vulkan/vulkan_instance.h
        include/graphics/v1/vulkan/vulkan_validation.h
        include/graphics/v1/vulkan/vulkan_swapchain.h
        include/graphics/v1/vulkan/vulkan_renderpasses.h
        include/graphics/v1/vulkan/vulkan_imgui.h
        include/graphics/v1/vulkan/vulkan_state.h
        include/graphics/v1/vulkan/pnext_chain_reader.h

        include/io/input_manager.h
        include/io/window.h
        include/io/logger.h

        include/ecs/entity_manager.h
        include/ecs/system_manager.h
        include/ecs/entity_system_manager.h
        include/ecs/entities/dense_sphere.h
        include/ecs/entities/camera.h
        include/ecs/entities/input_state_entity.h
        include/ecs/entities/monkey.h
        include/ecs/components/render_mesh.h
        include/ecs/components/render_mesh_simplifiable.h
        include/ecs/components/input_state.h
        include/ecs/components/rotating_sphere.h
        include/ecs/components/projector.h
        include/ecs/components/ui_state.h
        include/ecs/systems/camera_controller.h
        include/ecs/systems/sphere_controller.h
        include/ecs/systems/mesh_simplifier_controller.h
        include/ecs/entities/ui_state_entity.h

        include/physics/transformer.h

        include/util/timer.h
        include/util/importer.h
        include/util/byte_size.h
        include/util/performance_logging.h
        include/util/os.h
)
message(STATUS "HEADER_FILES: ${HEADER_FILES}")
set(SOURCE_FILES
        source/application.cpp

        source/core/scheduler.cpp

        source/graphics/colors.cpp

        source/graphics/v1/ui.cpp
        source/graphics/v1/renderer/drawing.cpp
        source/graphics/v1/renderer/common.cpp
        source/graphics/v1/renderer/shaders.cpp
        source/graphics/v1/renderer/systems.cpp
        source/graphics/v1/renderer/ui.cpp

        source/graphics/v1/vulkan/vulkan_buffers.cpp
        source/graphics/v1/vulkan/vulkan_images.cpp
        source/graphics/v1/vulkan/vulkan_memory.cpp
        source/graphics/v1/vulkan/vulkan_devices.cpp
        source/graphics/v1/vulkan/vulkan_instance.cpp
        source/graphics/v1/vulkan/vulkan_validation.cpp
        source/graphics/v1/vulkan/vulkan_swapchain.cpp
        source/graphics/v1/vulkan/vulkan_renderpasses.cpp
        source/graphics/v1/vulkan/vulkan_imgui.cpp

        source/io/input_manager.cpp
        source/io/window.cpp
        source/io/logger.cpp

        source/ecs/components/projector.cpp
        source/ecs/systems/camera_controller.cpp
        source/ecs/systems/sphere_controller.cpp
        source/ecs/systems/mesh_simplifier_controller.cpp
        source/ecs/entity_manager.cpp
        source/ecs/system_manager.cpp
        source/ecs/entities/camera.cpp
        source/ecs/entities/dense_sphere.cpp
        source/ecs/entities/input_state_entity.cpp
        source/ecs/entities/monkey.cpp
        source/ecs/entities/ui_state_entity.cpp

        source/physics/transformer.cpp

        source/util/importer.cpp
        source/util/timer.cpp
        source/util/performance_logging.cpp
)
message(STATUS "SOURCE_FILES: ${SOURCE_FILES}")
# Main executable
add_library(Doughnut STATIC source/main.cpp ${HEADER_FILES} ${SOURCE_FILES})

# Add folders
target_include_directories(Doughnut PUBLIC include)

# ----------------------------------- ADD DEPENDENCIES -----------------------------------

# assimp
add_subdirectory(external/assimp)
target_link_libraries(Doughnut PRIVATE assimp::assimp)
# glfw
add_subdirectory(external/glfw)
target_link_libraries(Doughnut PRIVATE glfw)
# vulkan
add_subdirectory(external/vulkan-headers)
target_link_libraries(Doughnut PRIVATE Vulkan::Headers)
# imgui
include(imgui.cmake)
target_link_libraries(Doughnut PRIVATE imgui)
# stb
target_include_directories(Doughnut PRIVATE external/stb)
# glm
add_subdirectory(external/glm)
target_link_libraries(Doughnut PRIVATE glm::glm)

# output
get_target_property(LL Doughnut LINK_LIBRARIES)
message(STATUS "Linked libraries: ${LL}")

# ----------------------------------- ADD RESOURCES -----------------------------------

# add resources
add_custom_target(CopyAssets COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/CopyAssets.cmake)
add_dependencies(Doughnut CopyAssets)