cmake_minimum_required(VERSION 3.18)
project(kstd-streams LANGUAGES C CXX)

option(KSTD_STREAMS_BUILD_TESTS "Build unit tests for kstd-streams" ON)
set(KSTD_STREAMS_MODULE ON) # Let other modules know we are present!
add_compile_definitions(KSTD_STREAMS_MODULE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;")
include(CommonLibraries)
include(Platform)

if(${KSTD_STREAMS_BUILD_TESTS})
    file(GLOB_RECURSE KSTD_SOURCE_FILES "${CMAKE_SOURCE_DIR}/test/*.cpp")
    add_executable(${CMAKE_PROJECT_NAME} ${KSTD_SOURCE_FILES})
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include")
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/kstd-types/include")
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/kstd-concepts/include")
    target_include_gtest(${CMAKE_PROJECT_NAME})
endif()