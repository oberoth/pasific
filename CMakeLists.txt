cmake_minimum_required(VERSION 3.5)
project(pasific)

include_directories(${CMAKE_SOURCE_DIR}/include)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_subdirectory(test)
