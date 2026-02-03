function(add_format_targets)
    
    # 1. C++ Formatting (clang-format)
    find_program(CLANG_FORMAT_EXE clang-format)
    if(CLANG_FORMAT_EXE)
        file(GLOB_RECURSE SOURCE_FILES 
             "${CMAKE_SOURCE_DIR}/src/*.cpp" 
             "${CMAKE_SOURCE_DIR}/src/*.h"
             "${CMAKE_SOURCE_DIR}/tests/*.cpp"
             "${CMAKE_SOURCE_DIR}/tests/*.h"
        )
        
        add_custom_target(format
            COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${SOURCE_FILES}
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Running clang-format on source and test files..."
            VERBATIM
        )
    else()
        message(STATUS "clang-format not found. Target 'format' not available.")
    endif()

    # 2. CMake Formatting (cmake-format)
    find_program(CMAKE_FORMAT_EXE cmake-format)
    if(CMAKE_FORMAT_EXE)
        # List specific CMake files to format (avoids globbing into build directories)
        set(CMAKE_TARGET_FILES
            "${CMAKE_SOURCE_DIR}/CMakeLists.txt"
            "${CMAKE_SOURCE_DIR}/src/CMakeLists.txt"
            "${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt"
        )
        file(GLOB MODULE_FILES "${CMAKE_SOURCE_DIR}/cmake/*.cmake")
        list(APPEND CMAKE_TARGET_FILES ${MODULE_FILES})

        add_custom_target(format-cmake
            COMMAND ${CMAKE_FORMAT_EXE} -i ${CMAKE_TARGET_FILES}
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Running cmake-format..."
            VERBATIM
        )
    else()
        # cmake-format often installed via pip, might not be in path
        message(STATUS "cmake-format not found. Target 'format-cmake' not available.")
    endif()

endfunction()

add_format_targets()
