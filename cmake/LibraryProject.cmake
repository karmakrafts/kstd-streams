# @author Alexander Hinze
# @since 20/01/2023

include(TemplateProject)

set(LIB_NAME ${CMAKE_PROJECT_NAME})

set(LIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE LIB_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE LIB_TEST_SOURCES ${CMAKE_SOURCE_DIR}/test/*.cpp)

# Macros
macro(lib_define_static_target)
    set(LIB_STATIC_TARGET ${LIB_NAME})
    # Static library
    add_library(${LIB_STATIC_TARGET} STATIC ${LIB_SOURCE_FILES})
    target_include_directories(${LIB_STATIC_TARGET} PUBLIC ${LIB_INCLUDE_DIR})

    if (PLATFORM_WINDOWS)
        # Make sure to link all relevant Win32 API libs, since a lot of external deps need those
        target_link_libraries(${LIB_STATIC_TARGET} kernel32 user32 ws2_32 Onecore)
    endif ()
endmacro()

macro(lib_define_shared_target)
    set(LIB_SHARED_TARGET "${LIB_NAME}0")
    # Dynamic Library
    add_library(${LIB_SHARED_TARGET} SHARED ${LIB_SOURCE_FILES})
    target_include_directories(${LIB_SHARED_TARGET} PUBLIC ${LIB_INCLUDE_DIR})

    if (PLATFORM_WINDOWS)
        # Make sure to link all relevant Win32 API libs, since a lot of external deps need those
        target_link_libraries(${LIB_SHARED_TARGET} kernel32 user32 ws2_32 Onecore)
    endif ()
endmacro()

macro(lib_define_test_target)
    set(LIB_TEST_TARGET "${LIB_NAME}_test")
    # Tests
    add_executable(${LIB_TEST_TARGET} ${LIB_TEST_SOURCES})
    target_include_gtest(${LIB_TEST_TARGET})
    target_include_directories(${LIB_TEST_TARGET} PUBLIC ${LIB_INCLUDE_DIR})
    target_link_libraries(${LIB_TEST_TARGET} ${CMAKE_PROJECT_NAME})
    add_dependencies(${LIB_TEST_TARGET} ${CMAKE_PROJECT_NAME})
endmacro()

macro(lib_define_targets)
    lib_define_shared_target()
    lib_define_static_target()
    lib_define_test_target()
endmacro()

macro(lib_maven_dependency url group name version)
    if(DEFINED LIB_STATIC_TARGET)
        target_maven_dependency(${LIB_STATIC_TARGET} ${url} ${group} ${name} ${version})
    endif()
    if(DEFINED LIB_SHARED_TARGET)
        target_maven_dependency(${LIB_SHARED_TARGET} ${url} ${group} ${name} ${version})
    endif()
    if(DEFINED LIB_TEST_TARGET)
        target_maven_dependency(${LIB_TEST_TARGET} ${url} ${group} ${name} ${version})
    endif()
endmacro()

macro(lib_include_directories)
    if(DEFINED LIB_STATIC_TARGET)
        target_include_directories(${LIB_STATIC_TARGET} ${ARGN})
    endif()
    if(DEFINED LIB_SHARED_TARGET)
        target_include_directories(${LIB_SHARED_TARGET} ${ARGN})
    endif()
    if(DEFINED LIB_TEST_TARGET)
        target_include_directories(${LIB_TEST_TARGET} ${ARGN})
    endif()
endmacro()

macro(lib_link_libraries)
    if(DEFINED LIB_STATIC_TARGET)
        target_link_libraries(${LIB_STATIC_TARGET} ${ARGN})
    endif()
    if(DEFINED LIB_SHARED_TARGET)
        target_link_libraries(${LIB_SHARED_TARGET} ${ARGN})
    endif()
    if(DEFINED LIB_TEST_TARGET)
        target_link_libraries(${LIB_TEST_TARGET} ${ARGN})
    endif()
endmacro()