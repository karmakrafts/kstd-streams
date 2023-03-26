# @author Alexander Hinze
# @since 04/01/2023

include(IncludeGuard)
include_guard_start(MAVEN_REPO)

include(Platform) # Need platform detection for determining library names

macro(target_maven_dependency target repo group name version)
    string(REGEX REPLACE "\\." "/" url_group ${group})
    set(url_base "${repo}/${url_group}/${name}/${version}")

    set(classifier "${BUILD_TYPE}-${PLATFORM_PAIR}")
    set(file_name "${name}-${version}-${classifier}.zip")
    message(STATUS "Resolving Maven dependency ${group}:${name}:${version}:${classifier}")

    FetchContent_Declare(${name} URL
            "${url_base}/${file_name}"
            DOWNLOAD_EXTRACT_TIMESTAMP NEW)

    FetchContent_MakeAvailable(${name})

    set(location "${CMAKE_CURRENT_BINARY_DIR}/_deps/${name}-src")
    set(include_location "${location}/include")
    set(static_library_location "${location}/${PLATFORM_LIB_PREFIX}${name}.${PLATFORM_SLIB_EXT}")
    set(dynamic_library_location "${location}/${PLATFORM_LIB_PREFIX}${name}0.${PLATFORM_DLIB_EXT}")
    set(cmake_location "${location}/cmake")

    if (EXISTS ${cmake_location})
        set(CMAKE_MODULE_PATH "${cmake_location};") # Include cmake modules if present
        message(STATUS "Including CMake scripts from ${cmake_location}")
    endif ()

    if (EXISTS ${include_location}) # If we have API headers, include them
        target_include_directories(${target} PUBLIC ${include_location})
        message(STATUS "Including headers from ${include_location}")
    endif ()

    if (EXISTS ${static_library_location}) # If we have a static lib, link it
        target_link_libraries(${target} ${static_library_location})
        message(STATUS "Linking library from ${static_library_location}")
    endif ()

    if (EXISTS ${dynamic_library_location}) # If we have a dynamic lib, copy to output
        configure_file(${dynamic_library_location} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
        message(STATUS "Copying shared library from ${dynamic_library_location}")
    endif ()

    unset(classifier)
    unset(cmake_location)
    unset(static_library_location)
    unset(dynamic_library_location)
    unset(include_location)
    unset(location)
    unset(url_group)
    unset(url_base)
    unset(file_name)
endmacro()

include_guard_end(MAVEN_REPO)