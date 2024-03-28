
include(FetchContent)

# ---------------------------------------- VULKAN ----------------------------------------

find_package(Vulkan REQUIRED)
message(STATUS "Found vulkan: ${VULKAN_FOUND}")
if (${VULKAN_FOUND})
    message("\tVulkan_LIBRARIES: ${Vulkan_LIBRARIES}")
    message("\tVulkan_INCLUDE_DIRS: ${Vulkan_INCLUDE_DIRS}")
endif ()

#set(Vulkan_LIBRARIES ${Vulkan_LIBRARIES} PARENT_SCOPE)
#set(Vulkan_INCLUDE_DIRS ${Vulkan_INCLUDE_DIRS} PARENT_SCOPE)

# ---------------------------------------- SDL ----------------------------------------

FetchContent_Declare(
        sdl
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG        0d8ce4a761a8e84e5b746d0d53e7b1dd8fc92d4d # SDL2 specifically
)
FetchContent_MakeAvailable(sdl)

# ---------------------------------------- IMGUI ----------------------------------------

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG        085781f5ca5372d5fc804d7e44b5bf27a8994af7 # Docking branch
)
FetchContent_MakeAvailable(imgui)
set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR}/backends)

target_include_directories(imgui PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(imgui PRIVATE ${Vulkan_LIBRARIES})

target_link_libraries(imgui PRIVATE SDL2::SDL2)

# ---------------------------------------- STB ----------------------------------------

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb
        GIT_TAG        ae721c50eaf761660b4f90cc590453cdb0c2acd0
)
FetchContent_MakeAvailable(stb)
set(STB_INCLUDE_DIR ${stb_SOURCE_DIR})

# ---------------------------------------- ASSIMP ----------------------------------------

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp
        GIT_TAG        feb861f17bf937fd42e0591b3347b95009033eec
)
set(ASSIMP_BUILD_TESTS OFF)
FetchContent_MakeAvailable(assimp)