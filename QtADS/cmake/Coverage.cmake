# Coverage.cmake
# Code coverage support for GCC/Clang using gcov/lcov/llvm-cov
#
# Usage:
#   include(Coverage)
#   enable_coverage(target_name)  # Add coverage flags to a target
#
# After running tests, generate report with:
#   cmake --build build --target coverage
#

option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")

    # Check compiler support
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # GCC/Clang coverage flags (must be a list, not a single string)
        set(COVERAGE_COMPILE_FLAGS -fprofile-arcs -ftest-coverage)
        set(COVERAGE_LINK_FLAGS -fprofile-arcs -ftest-coverage)

        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            # Clang uses different flags for newer versions
            set(COVERAGE_COMPILE_FLAGS --coverage)
            set(COVERAGE_LINK_FLAGS --coverage)
        endif()
    elseif(MSVC)
        message(WARNING "Code coverage is not well supported on MSVC. Consider using OpenCppCoverage externally.")
        set(COVERAGE_COMPILE_FLAGS "")
        set(COVERAGE_LINK_FLAGS "")
    else()
        message(WARNING "Code coverage not supported for compiler: ${CMAKE_CXX_COMPILER_ID}")
        set(COVERAGE_COMPILE_FLAGS "")
        set(COVERAGE_LINK_FLAGS "")
    endif()
endif()

# Function to enable coverage on a target
function(enable_coverage target)
    if(NOT ENABLE_COVERAGE)
        return()
    endif()

    if(NOT TARGET ${target})
        message(WARNING "enable_coverage: Target '${target}' does not exist")
        return()
    endif()

    message(STATUS "Enabling coverage for target: ${target}")

    target_compile_options(${target} PRIVATE ${COVERAGE_COMPILE_FLAGS})
    target_link_options(${target} PRIVATE ${COVERAGE_LINK_FLAGS})
endfunction()

# Function to add coverage report generation target
function(add_coverage_target)
    if(NOT ENABLE_COVERAGE)
        return()
    endif()

    # Find coverage tools
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    find_program(GCOV_PATH gcov)
    find_program(LLVM_COV_PATH llvm-cov)

    set(COVERAGE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/coverage")
    set(COVERAGE_INFO_FILE "${COVERAGE_OUTPUT_DIR}/coverage.info")
    set(COVERAGE_HTML_DIR "${COVERAGE_OUTPUT_DIR}/html")

    if(LCOV_PATH AND GENHTML_PATH)
        message(STATUS "Found lcov: ${LCOV_PATH}")
        message(STATUS "Found genhtml: ${GENHTML_PATH}")

        # Determine gcov executable
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND LLVM_COV_PATH)
            # llvm-cov must be called as "llvm-cov gcov", but lcov --gcov-tool
            # expects a single executable. Create a wrapper script.
            set(GCOV_WRAPPER "${CMAKE_BINARY_DIR}/llvm-gcov.sh")
            file(WRITE "${GCOV_WRAPPER}" "#!/bin/sh\nexec \"${LLVM_COV_PATH}\" gcov \"$@\"\n")
            file(CHMOD "${GCOV_WRAPPER}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
            set(GCOV_TOOL "${GCOV_WRAPPER}")
        elseif(GCOV_PATH)
            set(GCOV_TOOL "${GCOV_PATH}")
        else()
            set(GCOV_TOOL "gcov")
        endif()

        # Create coverage target using lcov
        add_custom_target(coverage
            # Create output directory
            COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_OUTPUT_DIR}

            # Reset counters
            COMMAND ${LCOV_PATH} --zerocounters --directory ${CMAKE_BINARY_DIR}

            # Run tests
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure

            # Capture coverage data
            COMMAND ${LCOV_PATH}
                --capture
                --directory ${CMAKE_BINARY_DIR}
                --output-file ${COVERAGE_INFO_FILE}
                --gcov-tool ${GCOV_TOOL}
                --ignore-errors mismatch

            # Remove unwanted paths (system headers, tests, external libs)
            COMMAND ${LCOV_PATH}
                --remove ${COVERAGE_INFO_FILE}
                '/usr/*'
                '*/tests/*'
                '*/test/*'
                '*/_deps/*'
                '*/googletest/*'
                '*/gtest/*'
                '*/Qt*/*'
                --output-file ${COVERAGE_INFO_FILE}
                --ignore-errors unused

            # Generate HTML report
            COMMAND ${GENHTML_PATH}
                ${COVERAGE_INFO_FILE}
                --output-directory ${COVERAGE_HTML_DIR}
                --title "${PROJECT_NAME} Code Coverage"
                --legend
                --show-details

            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report..."
            VERBATIM
        )

        # Add convenience target to open the report
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
            set(OPEN_CMD "open")
        elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
            set(OPEN_CMD "xdg-open")
        elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
            set(OPEN_CMD "start")
        endif()

        if(OPEN_CMD)
            add_custom_target(coverage-open
                COMMAND ${OPEN_CMD} ${COVERAGE_HTML_DIR}/index.html
                DEPENDS coverage
                COMMENT "Opening coverage report..."
            )
        endif()

        message(STATUS "Coverage target 'coverage' created. Run: cmake --build build --target coverage")
        message(STATUS "HTML report will be at: ${COVERAGE_HTML_DIR}/index.html")

    elseif(LLVM_COV_PATH AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Fallback to llvm-cov for Clang without lcov
        message(STATUS "Using llvm-cov for coverage (lcov not found)")

        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_OUTPUT_DIR}
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${LLVM_COV_PATH} report
                $<TARGET_FILE:${PROJECT_NAME}>
                -instr-profile=default.profdata
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report with llvm-cov..."
            VERBATIM
        )
    else()
        message(WARNING "Coverage tools not found. Install lcov and genhtml for coverage reports.")
        message(WARNING "  Ubuntu/Debian: sudo apt install lcov")
        message(WARNING "  macOS: brew install lcov")
        message(WARNING "  Fedora/RHEL: sudo dnf install lcov")
    endif()
endfunction()

# Setup coverage targets when included
if(ENABLE_COVERAGE)
    add_coverage_target()
endif()
