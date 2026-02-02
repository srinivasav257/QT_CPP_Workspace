# Environment Configuration Module
# 1. Loads key=value pairs from .env file into CMake variables
# 2. Generates a typed C++ header with these values

option(APP_ENVIRONMENT "Application Environment (Development, Staging, Production)" "Development")

# Internal list to track registered variables
set(REGISTERED_ENV_VARS "")

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
                    
                    # Set normal variable (local scope to function, so we must set PARENT_SCOPE or CACHE)
                    # We use CACHE to allow overriding from command line, but generic Set to allow internal overriding
                    if(NOT DEFINED ${KEY})
                         set(${KEY} "${VALUE}" PARENT_SCOPE)
                         set(${KEY} "${VALUE}" CACHE STRING "Loaded from .env" FORCE)
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
    set(${NAME} "${DEFAULT_VAL}" CACHE STRING "${DOC}")
    
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

    # Join lines
    string(REPLACE ";" "\n    " ENV_VAR_DECLARATIONS "${REGISTERED_ENV_VARS}")

    # Hack to allow boolean replacement in configure_file for our template
    # We replace _BASIC_CONSTEXPR_BOOL(@IS_DEV@) manually if needed, 
    # but strictly speaking @VAR@ works if mapped to true/false text.
    # The template uses @IS_DEV@ directly.
    # We just need to ensure the variable IS_DEV is 'true' or 'false' string.
    
    # Special fix for string_view header if needed (already included in template)

    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/env_config.h.in"
        "${CMAKE_BINARY_DIR}/generated/env_config.h"
    )
    
    target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_BINARY_DIR}/generated")

endfunction()
