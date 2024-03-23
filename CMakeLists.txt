cmake_minimum_required(VERSION 3.24)
project(Doughnut)

set(CMAKE_CXX_STANDARD 20)

# ----------------------------------- SETTINGS -----------------------------------


# ----------------------------------- ADD LIBRARY -----------------------------------

# Add files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS source/*.cpp)

# Main library
add_library(Doughnut STATIC ${SOURCE_FILES})

# Add include
target_include_directories(Doughnut PUBLIC include)

# ----------------------------------- ADD DEPENDENCIES -----------------------------------

add_subdirectory(external)

target_include_directories(Doughnut PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(Doughnut PUBLIC ${Vulkan_LIBRARIES})

target_link_libraries(Doughnut PRIVATE SDL2::SDL2)

target_link_libraries(Doughnut PUBLIC imgui)

target_include_directories(Doughnut PUBLIC ${STB_INCLUDE_DIR}) # TODO make private

target_link_libraries(Doughnut PRIVATE assimp::assimp)

# output
include(util.cmake)
print_target_properties(Doughnut)

# ----------------------------------- ADD RESOURCES -----------------------------------

# add resources
add_custom_target(CopyAssets COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/CopyAssets.cmake)
add_dependencies(Doughnut CopyAssets)

# ----------------------------------- ADD TEST EXECUTABLE -----------------------------------

enable_testing()

file(GLOB_RECURSE TESTS CONFIGURE_DEPENDS test_source/*.cpp)

add_executable(DoughnutTests ${TESTS})
target_include_directories(DoughnutTests PRIVATE test_include)
target_link_libraries(DoughnutTests PRIVATE Doughnut)

add_test(TestAll DoughnutTests)
