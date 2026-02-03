function(generate_auto_version)
    find_package(Git QUIET)

    # Defaults
    set(PROJECT_VERSION_MAJOR 0)
    set(PROJECT_VERSION_MINOR 1)
    set(PROJECT_VERSION_PATCH 0)
    set(PROJECT_VERSION_TWEAK "")
    set(PROJECT_GIT_HASH "unknown")
    set(PROJECT_VERSION_FULL "0.1.0-unknown")
    
    if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        # Get git hash
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE PROJECT_GIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # Get git description (tags)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty --match "v*"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE GIT_DESCRIBE_RESULT
        )

        if(GIT_DESCRIBE_RESULT EQUAL 0)
            set(PROJECT_VERSION_FULL "${GIT_DESCRIBE_VERSION}")
            
            # Regex to match v1.2.3-4-g123456
            if(GIT_DESCRIBE_VERSION MATCHES "^v?([0-9]+)\.([0-9]+)\.([0-9]+)(.*)$")
                set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})
                set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})
                set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})
                set(PROJECT_VERSION_TWEAK ${CMAKE_MATCH_4})
            endif()
        endif()
    endif()

    set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

    # Log version
    message(STATUS "Auto Version: ${PROJECT_VERSION}")
    message(STATUS "Full Version: ${PROJECT_VERSION_FULL}")
    message(STATUS "Git Hash:     ${PROJECT_GIT_HASH}")

    # Configure the file
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/version.h.in"
        "${CMAKE_BINARY_DIR}/generated/version.h"
        @ONLY
    )

    # Propagate version variables to parent scope
    set(PROJECT_VERSION "${PROJECT_VERSION}" PARENT_SCOPE)
    set(PROJECT_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}" PARENT_SCOPE)
    set(PROJECT_VERSION_MINOR "${PROJECT_VERSION_MINOR}" PARENT_SCOPE)
    set(PROJECT_VERSION_PATCH "${PROJECT_VERSION_PATCH}" PARENT_SCOPE)
    set(PROJECT_VERSION_TWEAK "${PROJECT_VERSION_TWEAK}" PARENT_SCOPE)
    set(PROJECT_VERSION_FULL "${PROJECT_VERSION_FULL}" PARENT_SCOPE)
    set(PROJECT_GIT_HASH "${PROJECT_GIT_HASH}" PARENT_SCOPE)
endfunction()

# Wrapper to link the generated header
function(target_include_version target_name)
    target_include_directories(${target_name} PRIVATE "${CMAKE_BINARY_DIR}/generated")
endfunction()

# Run immediately
generate_auto_version()
