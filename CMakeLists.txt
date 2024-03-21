cmake_minimum_required(VERSION 3.24)
project(Doughnut)

set(CMAKE_CXX_STANDARD 20)

# ----------------------------------- SETTINGS -----------------------------------


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

target_link_libraries(Doughnut PRIVATE SDL2::SDL2)

find_package(Vulkan REQUIRED)
message(STATUS "Found vulkan: ${VULKAN_FOUND}")
if (${VULKAN_FOUND})
    message("\tVulkan_LIBRARIES: ${Vulkan_LIBRARIES}")
    message("\tVulkan_INCLUDE_DIRS: ${Vulkan_INCLUDE_DIRS}")
endif ()
target_include_directories(Doughnut PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(Doughnut PUBLIC ${Vulkan_LIBRARIES})

target_link_libraries(Doughnut PUBLIC imgui)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(Doughnut PRIVATE assimp::assimp)
find_package(Stb REQUIRED)
target_include_directories(Doughnut PUBLIC ${Stb_INCLUDE_DIR})

# output
include(util.cmake)
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