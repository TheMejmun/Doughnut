cmake_minimum_required(VERSION 3.24)
project(Doughnut)

set(CMAKE_CXX_STANDARD 20)

# ----------------------------------- SETTINGS -----------------------------------

# https://stackoverflow.com/questions/32183975/how-to-print-all-the-properties-of-a-target-in-cmake
function(print_target_properties target)
    if (NOT CMAKE_PROPERTY_LIST)
        execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)

        # Convert command output into a CMake list
        string(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
        string(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
        list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)
    endif ()

    if (NOT TARGET ${target})
        message(STATUS "There is no target named '${target}'")
        return()
    endif ()

    message(STATUS "CMake target properties:")

    foreach (property ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "DEBUG" property ${property})

        if (${property} MATCHES "LOCATION")
            message(STATUS "Skipping LOCATION")
            continue()
        endif ()

        get_property(was_set TARGET ${target} PROPERTY ${property} SET)
        if (was_set)
            get_target_property(value ${target} ${property})
            message(STATUS "${target} ${property} = ${value}")
        endif ()
    endforeach ()
endfunction()

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

# TODO check if VK_FOLDER is even necessary or if it is unused.
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

# ----------------------------------- ADD LIBRARY -----------------------------------

# Add files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS source/*.cpp)
# message(STATUS "SOURCE_FILES: ${SOURCE_FILES}")

# Main library
add_library(Doughnut STATIC ${SOURCE_FILES})

# Add include
target_include_directories(Doughnut PUBLIC include)

# ----------------------------------- ADD DEPENDENCIES -----------------------------------

add_subdirectory(external)
#set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${VK_LIB})

#option(FETCH_SDL2 "Fetch and Compile SDL2" OFF)
#if (${FETCH_SDL2})
#    message(STATUS "Fetching SDL2")
#else ()
#    message(STATUS "Linking Existing SDL2")
#    find_package(SDL2 CONFIG REQUIRED)
#    target_link_libraries(Doughnut PUBLIC ${SDL2_LIBRARIES})
#endif ()

# vcpkg SDL2
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(Doughnut
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

#target_link_directories(Doughnut PUBLIC ${VK_LIB})
target_include_directories(Doughnut PUBLIC ${VK_INCLUDE})

find_package(assimp CONFIG REQUIRED)
target_link_libraries(Doughnut PRIVATE assimp::assimp)
find_package(Stb REQUIRED)
target_include_directories(Doughnut PRIVATE ${Stb_INCLUDE_DIR})

find_package(imgui CONFIG REQUIRED)
target_link_libraries(Doughnut PUBLIC imgui::imgui)

# output
print_target_properties(Doughnut)

# ----------------------------------- ADD RESOURCES -----------------------------------

# add resources
add_custom_target(CopyAssets COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/CopyAssets.cmake)
add_dependencies(Doughnut CopyAssets)

# ----------------------------------- ADD TEST EXECUTABLE -----------------------------------

get_target_property(TEST_LINK_LIBRARIES Doughnut LINK_LIBRARIES)
get_target_property(TEST_INCLUDE_DIRS Doughnut INCLUDE_DIRECTORIES)
get_target_property(TEST_SOURCES Doughnut SOURCES)

add_executable(DoughnutTests source/test.cpp ${TEST_SOURCES})
target_include_directories(DoughnutTests PRIVATE ${TEST_INCLUDE_DIRS})
target_link_libraries(DoughnutTests PRIVATE ${TEST_LINK_LIBRARIES})

# https://stackoverflow.com/questions/32901679/unit-testing-as-part-of-the-build

#enable_testing()
#
##add_dependencies(DoughnutTests Doughnut)
#
#add_test(TestsBuild
#        "${CMAKE_COMMAND}"
#        --build "${CMAKE_BINARY_DIR}"
#        --config "$<CONFIG>"
#        --target DoughnutTests
#)
#set_tests_properties(TestsBuild PROPERTIES FIXTURES_SETUP TestsFixture)
#add_test(Doughnut DoughnutTests)
#set_tests_properties(Doughnut PROPERTIES FIXTURES_REQUIRED TestsFixture)
#
#add_custom_command(
#        TARGET Doughnut
#        COMMENT "Run tests"
#        POST_BUILD
#        COMMAND ctest
#        ARGS -C $<CONFIGURATION> -R "^DoughnutTests$" --output-on-failures
#)