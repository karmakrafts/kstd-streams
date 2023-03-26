cmake_minimum_required(VERSION 3.18)
project(cxxstreams LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;")
include(CommonLibraries)
include(Platform)

include_scripts()

file(GLOB_RECURSE CXXS_SOURCE_FILES "${CMAKE_SOURCE_DIR}/test/*.cpp")
add_executable(${CMAKE_PROJECT_NAME} ${CXXS_SOURCE_FILES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_gtest(${CMAKE_PROJECT_NAME})