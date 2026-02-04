# Environment Configuration Module
# 1. Loads key=value pairs from .env file into CMake variables
# 2. Generates a typed C++ header with these values

set(APP_ENVIRONMENT "Development" CACHE STRING "Application Environment (Development, Staging, Production)")
set_property(CACHE APP_ENVIRONMENT PROPERTY STRINGS "Development" "Staging" "Production")

# Internal list to track registered variables
set(REGISTERED_ENV_VARS "")

# Helper: normalize CMake booleans (ON/OFF/TRUE/FALSE/1/0) to C++ true/false
function(normalize_cmake_bool input output_var)
    string(TOUPPER "${input}" _upper)
    if(_upper MATCHES "^(1|ON|TRUE|YES)$")
        set(${output_var} "true" PARENT_SCOPE)
    else()
        set(${output_var} "false" PARENT_SCOPE)
    endif()
endfunction()

# Function: load_dotenv()
# Reads .env file from sourcedir and sets CMake variables
function(load_dotenv)
    set(DOTENV_FILE "${CMAKE_SOURCE_DIR}/.env")
    if(EXISTS "${DOTENV_FILE}")
        message(STATUS "EnvConfig: Loading ${DOTENV_FILE}...")
        file(STRINGS "${DOTENV_FILE}" ENV_LINES)

        foreach(LINE ${ENV_LINES})
            # Ignore comments and empty lines
            string(REGEX MATCH "^[ \t]*#" IS_COMMENT "${LINE}")
            string(REGEX MATCH "^[ \t]*$" IS_EMPTY "${LINE}")

            if(NOT IS_COMMENT AND NOT IS_EMPTY)
                # Split by first =
                string(FIND "${LINE}" "=" EQUAL_POS)
                if(EQUAL_POS GREATER -1)
                    string(SUBSTRING "${LINE}" 0 ${EQUAL_POS} KEY)
                    math(EXPR VAL_START "${EQUAL_POS} + 1")
                    string(SUBSTRING "${LINE}" ${VAL_START} -1 VALUE)

                    string(STRIP "${KEY}" KEY)
                    string(STRIP "${VALUE}" VALUE)

                    # Validate key name to prevent injection
                    if(NOT KEY MATCHES "^[A-Za-z_][A-Za-z0-9_]*$")
                        message(FATAL_ERROR "EnvConfig: Invalid .env key: '${KEY}'")
                    endif()

                    # Only set if not already defined (respects command-line overrides)
                    if(NOT DEFINED ${KEY})
                         set(${KEY} "${VALUE}" PARENT_SCOPE)
                         set(${KEY} "${VALUE}" CACHE STRING "Loaded from .env")
                    endif()
                endif()
            endif()
        endforeach()
    endif()
endfunction()

# Function: add_config_var
# Register a variable to be included in the generated header
# Types: STRING, INT, BOOL
function(add_config_var NAME TYPE DEFAULT_VAL DOC)
    if("${TYPE}" STREQUAL "BOOL")
        # Normalize default to true/false for C++ compatibility
        normalize_cmake_bool("${DEFAULT_VAL}" _normalized)
        set(${NAME} "${_normalized}" CACHE STRING "${DOC}")
    else()
        set(${NAME} "${DEFAULT_VAL}" CACHE STRING "${DOC}")
    endif()

    # Store definition for generation
    if("${TYPE}" STREQUAL "STRING")
        list(APPEND REGISTERED_ENV_VARS "static constexpr std::string_view ${NAME} = \"@${NAME}@\";")
    elseif("${TYPE}" STREQUAL "INT")
        list(APPEND REGISTERED_ENV_VARS "static constexpr int ${NAME} = @${NAME}@;")
    elseif("${TYPE}" STREQUAL "BOOL")
        list(APPEND REGISTERED_ENV_VARS "static constexpr bool ${NAME} = @${NAME}@;")
    else()
        message(WARNING "Unknown env var type: ${TYPE}")
    endif()

    set(REGISTERED_ENV_VARS ${REGISTERED_ENV_VARS} PARENT_SCOPE)
endfunction()

# Function: generate_env_header
# Generates the header and links it to target
function(generate_env_header TARGET_NAME)

    # Helper for IS_DEV
    if("${APP_ENVIRONMENT}" STREQUAL "Development")
        set(IS_DEV "true")
    else()
        set(IS_DEV "false")
    endif()

    # Normalize any BOOL cache vars that may still have ON/OFF values
    foreach(_decl ${REGISTERED_ENV_VARS})
        if(_decl MATCHES "constexpr bool ([A-Za-z_][A-Za-z0-9_]*)")
            set(_var_name "${CMAKE_MATCH_1}")
            normalize_cmake_bool("${${_var_name}}" _normalized)
            set(${_var_name} "${_normalized}")
        endif()
    endforeach()

    # Join lines
    string(REPLACE ";" "\n    " ENV_VAR_DECLARATIONS "${REGISTERED_ENV_VARS}")

    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/env_config.h.in"
        "${CMAKE_BINARY_DIR}/generated/env_config.h"
    )

    target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_BINARY_DIR}/generated")

endfunction()
