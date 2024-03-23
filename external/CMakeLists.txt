
# ---------------------------------------- VULKAN ----------------------------------------

find_package(Vulkan REQUIRED)
message(STATUS "Found vulkan: ${VULKAN_FOUND}")
if (${VULKAN_FOUND})
    message("\tVulkan_LIBRARIES: ${Vulkan_LIBRARIES}")
    message("\tVulkan_INCLUDE_DIRS: ${Vulkan_INCLUDE_DIRS}")
endif ()

set(Vulkan_LIBRARIES ${Vulkan_LIBRARIES} PARENT_SCOPE)
set(Vulkan_INCLUDE_DIRS ${Vulkan_INCLUDE_DIRS} PARENT_SCOPE)

# ---------------------------------------- SDL ----------------------------------------

add_subdirectory(sdl)

# ---------------------------------------- IMGUI ----------------------------------------

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
target_include_directories(imgui PUBLIC imgui/backends)

target_include_directories(imgui PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(imgui PRIVATE ${Vulkan_LIBRARIES})

target_link_libraries(imgui PRIVATE SDL2::SDL2)

include(../util.cmake)
print_target_properties(imgui)

# ---------------------------------------- STB ----------------------------------------

set(STB_INCLUDE_DIR "external/stb" PARENT_SCOPE)

# ---------------------------------------- ASSIMP ----------------------------------------

set(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(assimp)