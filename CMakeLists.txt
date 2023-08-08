cmake_minimum_required(VERSION 3.18)
project(kstd-streams LANGUAGES C CXX)

option(KSTD_STREAMS_BUILD_TESTS "Build unit tests for kstd-streams" OFF)
set(KSTD_STREAMS_MODULE ON) # Let other modules know we are present!
add_compile_definitions(KSTD_STREAMS_MODULE)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;")
include(cmx)

if(${KSTD_STREAMS_BUILD_TESTS})
	cmx_include_scripts()
    cmx_add_tests(kstd-streams-tests "${CMAKE_SOURCE_DIR}/test")
    target_include_directories(kstd-streams-tests PUBLIC "${CMAKE_SOURCE_DIR}/include")
    cmx_include_gtest(kstd-streams-tests PRIVATE)
    cmx_include_kstd_core(kstd-streams-tests PRIVATE)
endif()
