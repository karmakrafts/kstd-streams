if (NOT CMX_INCLUDED)
    include(FetchContent)
    set(CMX_VERSION master)

    FetchContent_Declare(
        cmx
        GIT_REPOSITORY https://github.com/karmakrafts/cmx.git
        GIT_TAG ${CMX_VERSION}
    )
    FetchContent_MakeAvailable(cmx)
    set(CMAKE_MODULE_PATH "${cmx_SOURCE_DIR};")
    include(cmx)

    set(CMX_INCLUDED ON)
endif ()# CMX_INCLUDED
