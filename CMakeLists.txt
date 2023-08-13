cmake_minimum_required(VERSION 3.18)
project(kstd-streams LANGUAGES C CXX)

option(KSTD_STREAMS_BUILD_TESTS "Build unit tests for kstd-streams" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;")
include(cmx)
cmx_include_scripts()

cmx_add_library(kstd-streams INTERFACE)
cmx_include_kstd_core(kstd-streams INTERFACE)
target_include_directories(kstd-streams INTERFACE "${CMAKE_SOURCE_DIR}/include")

if(${KSTD_STREAMS_BUILD_TESTS})
    cmx_add_tests(kstd-streams-tests "${CMAKE_SOURCE_DIR}/test")
    cmx_include_gtest(kstd-streams-tests PRIVATE)
	target_link_libraries(kstd-streams-tests PRIVATE kstd-streams)
	add_dependencies(kstd-streams-tests kstd-streams)
endif()
