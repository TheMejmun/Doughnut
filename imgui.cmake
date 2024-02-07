# imgui
file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS external/imgui/*.cpp)
set(IMGUI_SOURCES
        ${IMGUI_SOURCES}
        external/imgui/backends/imgui_impl_vulkan.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/misc/cpp/imgui_stdlib.cpp)
message(STATUS "IMGUI_SOURCES: ${IMGUI_SOURCES}")

file(GLOB IMGUI_HEADERS CONFIGURE_DEPENDS external/imgui/*.h)
set(IMGUI_HEADERS
        ${IMGUI_HEADERS}
        external/imgui/backends/imgui_impl_vulkan.h
        external/imgui/backends/imgui_impl_glfw.h
        external/imgui/misc/cpp/imgui_stdlib.h)
message(STATUS "IMGUI_HEADERS: ${IMGUI_HEADERS}")

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(imgui PUBLIC
        external/imgui
        external/imgui/misc/cpp
        external/imgui/backends)

target_link_libraries(imgui PRIVATE glfw)
target_link_libraries(imgui PRIVATE Vulkan::Headers)