cmake_minimum_required(VERSION 3.18)
project(kstd-streams LANGUAGES C CXX)

option(KSTD_STREAMS_BUILD_TESTS "Build unit tests for kstd-streams" ON)
set(KSTD_STREAMS_MODULE ON) # Let other modules know we are present!
add_compile_definitions(KSTD_STREAMS_MODULE)

if(NOT DEFINED CMAKE_CXX_STANDARD)
	message(STATUS "No C++ version specified, defaulting to C++20")
	set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;")
include(Platform)
include(CommonLibraries)
include_scripts()

if(${KSTD_STREAMS_BUILD_TESTS})
    file(GLOB_RECURSE KSTD_TEST_SOURCE_FILES "${CMAKE_SOURCE_DIR}/test/*.cpp")
    add_executable(kstd-streams-tests ${KSTD_TEST_SOURCE_FILES})
    target_include_directories(kstd-streams-tests PUBLIC "${CMAKE_SOURCE_DIR}/kstd-core/include")
    target_include_directories(kstd-streams-tests PUBLIC "${CMAKE_SOURCE_DIR}/include")
    target_include_gtest(kstd-streams-tests)
endif()
