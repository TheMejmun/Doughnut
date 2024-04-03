
include(FetchContent)

option(SKIP_EXTERNAL_TESTS "Skip external dependency testing" ON)

# ---------------------------------------- VULKAN ----------------------------------------

find_package(Vulkan REQUIRED)
message(STATUS "Found vulkan: ${VULKAN_FOUND}")
if (${VULKAN_FOUND})
    message("\tVulkan_LIBRARIES: ${Vulkan_LIBRARIES}")
    message("\tVulkan_INCLUDE_DIRS: ${Vulkan_INCLUDE_DIRS}")
endif ()

# ---------------------------------------- SDL ----------------------------------------

FetchContent_Declare(
        sdl
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG 0d8ce4a761a8e84e5b746d0d53e7b1dd8fc92d4d # SDL2 specifically
)
if (${SKIP_EXTERNAL_TESTS})
    set(SDL_TEST OFF)
endif ()
FetchContent_MakeAvailable(sdl)

# ---------------------------------------- IMGUI ----------------------------------------

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG 085781f5ca5372d5fc804d7e44b5bf27a8994af7 # Docking branch
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
        GIT_TAG ae721c50eaf761660b4f90cc590453cdb0c2acd0
)
FetchContent_MakeAvailable(stb)
set(STB_INCLUDE_DIR ${stb_SOURCE_DIR})

# ---------------------------------------- ASSIMP ----------------------------------------

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp
        GIT_TAG feb861f17bf937fd42e0591b3347b95009033eec
)
if (${SKIP_EXTERNAL_TESTS})
    set(ASSIMP_BUILD_TESTS OFF)
endif ()
# Building assimp as a shared library does not seem to work with MSVC. Not sure why.
set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(assimp)

# ---------------------------------------- SPIR-V HEADERS ----------------------------------------

FetchContent_Declare(
        spirv-headers
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers
        GIT_TAG 4f7b471f1a66b6d06462cd4ba57628cc0cd087d7
)
FetchContent_MakeAvailable(spirv-headers)

# ---------------------------------------- SPIR-V TOOLS ----------------------------------------

FetchContent_Declare(
        spirv-tools
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools
        GIT_TAG 7fe5f75e581014e920ab5d9a218ea2f37bbaa0d4
)
#set(SPIRV_TOOLS_BUILD_STATIC OFF)
FetchContent_MakeAvailable(spirv-tools)

# ---------------------------------------- GLSLANG ----------------------------------------

FetchContent_Declare(
        glslang
        GIT_REPOSITORY https://github.com/KhronosGroup/glslang
        GIT_TAG 0015dc9345ff9572af60801948c82b7ebce5ddb3
)
if (${SKIP_EXTERNAL_TESTS})
    set(GLSLANG_TESTS OFF)
endif ()
#set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(glslang)

# https://github.com/KhronosGroup/glslang/issues/3509
add_library(glslang::glslang ALIAS glslang)
add_library(glslang::SPIRV ALIAS SPIRV)

# ---------------------------------------- SHADERC ----------------------------------------

FetchContent_Declare(
        shaderc
        GIT_REPOSITORY https://github.com/google/shaderc
        GIT_TAG 9a658e242ad4d1a4b3491383c1c58c780e3c01ff
)
if (${SKIP_EXTERNAL_TESTS})
    set(SHADERC_SKIP_TESTS ON)
endif ()
# MSVC does not link the CRT correctly. Setting this option fixes Windows builds.
set(SHADERC_ENABLE_SHARED_CRT ON)
FetchContent_MakeAvailable(shaderc)

# ---------------------------------------- TINY FILE DIALOG ----------------------------------------

FetchContent_Declare(
        tiny-file-dialogs
        GIT_REPOSITORY https://git.code.sf.net/p/tinyfiledialogs/code tinyfiledialogs-code
        GIT_TAG ddaf6ea8efb5d19ae39c9d59bbd3ebedf5c4b136
)
FetchContent_MakeAvailable(tiny-file-dialogs)

add_library(tiny-file-dialogs STATIC
        ${tiny-file-dialogs_SOURCE_DIR}/tinyfiledialogs.c
        ${tiny-file-dialogs_SOURCE_DIR}/more_dialogs/tinyfd_moredialogs.c)
target_include_directories(tiny-file-dialogs PUBLIC ${tiny-file-dialogs_SOURCE_DIR})
