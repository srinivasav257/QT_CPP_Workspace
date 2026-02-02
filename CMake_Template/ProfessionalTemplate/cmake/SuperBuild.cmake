include(ExternalProject)

# Define the install location for dependencies
set(DEPENDENCIES_INSTALL_DIR "${CMAKE_BINARY_DIR}/dependencies-install")
set(DEPENDENCIES_SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies-source")

# Pass the current build type and toolchain to the sub-projects
set(COMMON_CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
)

if(MSVC)
   list(APPEND COMMON_CMAKE_ARGS -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS})
endif()

# --- 1. fmt ---
ExternalProject_Add(sb_fmt
    PREFIX ${DEPENDENCIES_SOURCE_DIR}/fmt
    URL https://github.com/fmtlib/fmt/archive/refs/tags/10.1.1.zip
    CMAKE_ARGS ${COMMON_CMAKE_ARGS} -DFMT_TEST=OFF
    INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE} --target install
)

# --- 2. spdlog ---
ExternalProject_Add(sb_spdlog
    PREFIX ${DEPENDENCIES_SOURCE_DIR}/spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.zip
    CMAKE_ARGS ${COMMON_CMAKE_ARGS} -DSPDLOG_FMT_EXTERNAL=ON
    DEPENDS sb_fmt
    INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE} --target install
)

# --- 3. GoogleTest ---
ExternalProject_Add(sb_googletest
    PREFIX ${DEPENDENCIES_SOURCE_DIR}/googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    CMAKE_ARGS ${COMMON_CMAKE_ARGS} -Dgtest_force_shared_crt=ON
    INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE} --target install
)

# --- 4. Main Project ---
# This re-invokes CMake on the same source directory, but with ENABLE_SUPERBUILD=OFF
# and pointing to the installed dependencies.

EXTERNALPROCESS_AUTORUN=OFF

ExternalProject_Add(sb_main_project
    PREFIX ${CMAKE_BINARY_DIR}/main_project
    SOURCE_DIR ${CMAKE_SOURCE_DIR}
    BINARY_DIR ${CMAKE_BINARY_DIR}/main_project/build
    DEPENDS sb_fmt sb_spdlog sb_googletest
    INSTALL_COMMAND "" # Main project isn't "installed" in the SuperBuild sense, just built
    CMAKE_ARGS 
        ${COMMON_CMAKE_ARGS}
        -DENABLE_SUPERBUILD=OFF
        -DCMAKE_PREFIX_PATH=${DEPENDENCIES_INSTALL_DIR};${CMAKE_PREFIX_PATH}
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    BUILD_ALWAYS 1
)
