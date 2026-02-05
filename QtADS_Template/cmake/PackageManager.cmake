option(USE_CONAN "Enable Conan Package Manager" OFF)
option(USE_VCPKG "Enable Vcpkg Package Manager" OFF)

if(USE_CONAN)
    # 1. Verify Conan is installed
    find_program(CONAN_CMD conan)
    if(NOT CONAN_CMD)
        message(FATAL_ERROR "Conan executable not found! Please install conan to use USE_CONAN.")
    endif()

    # 2. Auto-generate conanfile.txt if missing
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt")
        message(STATUS "Generating default conanfile.txt...")
        file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt"
"[requires]
fmt/10.1.1
spdlog/1.12.0
gtest/1.14.0

[generators]
CMakeDeps
CMakeToolchain

[layout]
cmake_layout
")
    endif()

    # 3. Detect default profile if none exists
    execute_process(COMMAND ${CONAN_CMD} profile list
                    RESULT_VARIABLE CONAN_PROFILE_RESULT
                    OUTPUT_QUIET ERROR_QUIET)
    
    if(NOT CONAN_PROFILE_RESULT EQUAL 0)
        message(STATUS "No Conan profile detected. Running 'conan profile detect'...")
        execute_process(COMMAND ${CONAN_CMD} profile detect)
    endif()

    # 4. Run Conan Install
    if(CMAKE_CONFIGURATION_TYPES)
        # Multi-config generator (Visual Studio, Ninja Multi-Config): install for each configuration
        foreach(_cfg IN LISTS CMAKE_CONFIGURATION_TYPES)
            message(STATUS "Running Conan Install (Build Type: ${_cfg})...")
            execute_process(COMMAND ${CONAN_CMD} install . --build=missing -s build_type=${_cfg}
                            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                            RESULT_VARIABLE CONAN_INSTALL_RESULT)
            if(NOT CONAN_INSTALL_RESULT EQUAL 0)
                message(WARNING "Conan install failed for build type '${_cfg}'. Check output.")
            endif()
        endforeach()
    else()
        # Single-config generator: use CMAKE_BUILD_TYPE, defaulting to Release
        if(NOT CMAKE_BUILD_TYPE)
            set(_CONAN_BUILD_TYPE "Release")
        else()
            set(_CONAN_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
        endif()

        message(STATUS "Running Conan Install (Build Type: ${_CONAN_BUILD_TYPE})...")
        execute_process(COMMAND ${CONAN_CMD} install . --build=missing -s build_type=${_CONAN_BUILD_TYPE}
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                        RESULT_VARIABLE CONAN_INSTALL_RESULT)
        if(NOT CONAN_INSTALL_RESULT EQUAL 0)
            message(WARNING "Conan install failed. Check output.")
        endif()
    endif()

    # 5. Set Toolchain if generated
    # Note: Modern Conan generates 'conan_toolchain.cmake' in the build folder (depending on layout)
    # We attempt to find it to auto-hook it.
    file(GLOB_RECURSE FOUND_CONAN_TOOLCHAIN "${CMAKE_CURRENT_BINARY_DIR}/*/conan_toolchain.cmake")
    
    if(FOUND_CONAN_TOOLCHAIN)
        list(GET FOUND_CONAN_TOOLCHAIN 0 CONAN_TOOLCHAIN_PATH)
        if(NOT CMAKE_TOOLCHAIN_FILE)
             set(CMAKE_TOOLCHAIN_FILE "${CONAN_TOOLCHAIN_PATH}" CACHE FILEPATH "" FORCE)
             message(STATUS "Process: Assigned CMAKE_TOOLCHAIN_FILE to '${CONAN_TOOLCHAIN_PATH}'")
        endif()
    endif()

endif()

if(USE_VCPKG)
    if(NOT CMAKE_TOOLCHAIN_FILE)
        # Try to find VCPKG_ROOT env var
        if(DEFINED ENV{VCPKG_ROOT})
            set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "" FORCE)
            message(STATUS "Vcpkg: Detected VCPKG_ROOT. Toolchain set.")
        else()
             message(FATAL_ERROR "USE_VCPKG is ON, but VCPKG_ROOT environment variable is not set.")
        endif()
    endif()
endif()
