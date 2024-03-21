# https://stackoverflow.com/questions/32183975/how-to-print-all-the-properties-of-a-target-in-cmake
function(print_target_properties target)
    if (NOT CMAKE_PROPERTY_LIST)
        execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)

        # Convert command output into a CMake list
        string(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
        string(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
        list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)
    endif ()

    if (NOT TARGET ${target})
        message(STATUS "There is no target named '${target}'")
        return()
    endif ()

    message(STATUS "CMake target properties:")

    foreach (property ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "DEBUG" property ${property})

        if (${property} MATCHES "LOCATION")
            message(STATUS "Skipping LOCATION")
            continue()
        endif ()

        get_property(was_set TARGET ${target} PROPERTY ${property} SET)
        if (was_set)
            get_target_property(value ${target} ${property})
            message(STATUS "${target} ${property} = ${value}")
        endif ()
    endforeach ()
endfunction()