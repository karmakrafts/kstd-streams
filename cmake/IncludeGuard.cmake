# @author Alexander Hinze
# @since 04/01/2023

if (GUARD_INCLUDED)
    return()
endif ()

macro(include_guard_start name)
    if (${name}_INCLUDED)
        return()
    endif ()
endmacro()

macro(include_guard_end name)
    set(${name}_INCLUDED TRUE)
endmacro()

set(GUARD_INCLUDED TRUE)