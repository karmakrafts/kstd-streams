# @author Alexander Hinze
# @since 04/01/2023

include(IncludeGuard)
include_guard_start(COMMON_LIBS)

include(FetchContent)
include(Platform)
set(CL_DEPS_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps)

# CMake Scripts
set(CL_SCRIPTS_VERSION 22.01)
set(CL_SCRIPTS_FETCHED OFF)

macro(include_scripts)
    if (NOT CL_SCRIPTS_FETCHED)
        FetchContent_Declare(
                cmake_scripts
                GIT_REPOSITORY https://github.com/StableCoder/cmake-scripts.git
                GIT_TAG 22.01)
        FetchContent_Populate(cmake_scripts)

        set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}/_deps/cmake_scripts-src;")
        set(ENABLE_ALL_WARNINGS ON)

        include(compiler-options)

        if (NOT COMPILER_MSVC)
            set(USE_SANITIZER Thread,Undefined)
            include(sanitizers)
        endif ()

        set(CL_SCRIPTS_FETCHED ON)
    endif ()
endmacro()

# {fmt}
set(CL_FMT_VERSION 9.1.0)
set(CL_FMT_FETCHED OFF)

macro(target_include_fmt target)
    if (NOT CL_FMT_FETCHED)
        FetchContent_Declare(
                fmt
                GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                GIT_TAG ${CL_FMT_VERSION}
        )
        FetchContent_MakeAvailable(fmt)
        set(CL_FMT_FETCHED ON)
    endif ()

    target_compile_definitions(${target} PUBLIC FMT_HEADER_ONLY)
    target_include_directories(${target} PUBLIC ${CL_DEPS_DIR}/fmt-src/include)
endmacro()

macro(include_fmt)
    if (NOT CL_FMT_FETCHED)
        FetchContent_Declare(
                fmt
                GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                GIT_TAG ${CL_FMT_VERSION}
        )
        FetchContent_MakeAvailable(fmt)
        set(CL_FMT_FETCHED ON)
    endif ()

    add_compile_definitions(FMT_HEADER_ONLY)
    include_directories(${CL_DEPS_DIR}/fmt-src/include)
endmacro()

# parallel_hashmap
set(CL_PHMAP_VERSION 1.3.8)
set(CL_PHMAP_FETCHED OFF)

macro(target_include_phmap target)
    if (NOT CL_PHMAP_FETCHED)
        FetchContent_Declare(
                phmap
                GIT_REPOSITORY https://github.com/greg7mdp/parallel-hashmap.git
                GIT_TAG "v${CL_PHMAP_VERSION}"
        )
        FetchContent_MakeAvailable(phmap)
        set(CL_PHMAP_FETCHED ON)
    endif ()

    target_compile_definitions(${target} PUBLIC _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING)
    target_include_directories(${target} PUBLIC ${CL_DEPS_DIR}/phmap-src)
endmacro()

macro(include_phmap)
    if (NOT CL_PHMAP_FETCHED)
        FetchContent_Declare(
                phmap
                GIT_REPOSITORY https://github.com/greg7mdp/parallel-hashmap.git
                GIT_TAG "v${CL_PHMAP_VERSION}"
        )
        FetchContent_MakeAvailable(phmap)
        set(CL_PHMAP_FETCHED ON)
    endif ()

    add_compile_definitions(_SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING)
    include_directories(${CL_DEPS_DIR}/phmap-src)
endmacro()

# Atomic Queue
set(CL_ATOMIC_QUEUE_VERSION master) # This is fine for now..
set(CL_ATOMIC_QUEUE_FETCHED OFF)

macro(target_include_atomic_queue target)
    if (NOT CL_ATOMIC_QUEUE_FETCHED)
        FetchContent_Declare(
                atomic_queue
                GIT_REPOSITORY https://github.com/max0x7ba/atomic_queue.git
                GIT_TAG ${CL_ATOMIC_QUEUE_VERSION}
        )
        FetchContent_MakeAvailable(atomic_queue)
        set(CL_ATOMIC_QUEUE_FETCHED ON)
    endif ()

    target_include_directories(${target} PUBLIC ${CL_DEPS_DIR}/atomic_queue-src/include)
endmacro()

macro(include_atomic_queue)
    if (NOT CL_ATOMIC_QUEUE_FETCHED)
        FetchContent_Declare(
                atomic_queue
                GIT_REPOSITORY https://github.com/max0x7ba/atomic_queue.git
                GIT_TAG ${CL_ATOMIC_QUEUE_VERSION}
        )
        FetchContent_MakeAvailable(atomic_queue)
        set(CL_ATOMIC_QUEUE_FETCHED ON)
    endif ()

    include_directories(${CL_DEPS_DIR}/atomic_queue-src/include)
endmacro()

# GoogleTest
set(CL_GTEST_VERSION 1.13.0)
set(CL_GTEST_FETCHED OFF)

macro(target_include_gtest target)
    if (NOT CL_GTEST_FETCHED)
        FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG "v${CL_GTEST_VERSION}")
        set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)

        enable_testing(TRUE)
        include(GoogleTest)
        set(CL_GTEST_FETCHED ON)
    endif ()

    gtest_discover_tests(${target})
    target_link_libraries(${target} gtest_main)
endmacro()

# GLM
set(CL_GLM_VERSION 0.9.9.8)
set(CL_GLM_FETCHED OFF)

macro(target_include_glm target)
    if (NOT CL_GLM_FETCHED)
        FetchContent_Declare(
                glm
                GIT_REPOSITORY https://github.com/g-truc/glm.git
                GIT_TAG ${CL_GLM_VERSION})
        FetchContent_MakeAvailable(glm)
        set(CL_GLM_FETCHED ON)
    endif ()

    target_include_directories(${target} PUBLIC ${CL_DEPS_DIR}/glm-src)
endmacro()

include_guard_end(COMMON_LIBS)