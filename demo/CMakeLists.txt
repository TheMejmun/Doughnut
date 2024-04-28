cmake_minimum_required(VERSION 3.27)
project(DoughnutDemo)

set(CMAKE_CXX_STANDARD 20)

add_executable(DoughnutDemo main.cpp)

#add_subdirectory(../Doughnut)
target_link_libraries(DoughnutDemo Doughnut)
