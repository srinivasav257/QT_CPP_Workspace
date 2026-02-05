include(FetchContent)

# FetchDependencies.cmake
# Manages third-party dependencies: pre-compiled local libraries and fetched sources.
#
# Pre-compiled libraries (third_party/):
#   - GoogleTest (gtest, gmock) — shared/import libraries
#   - Qt Advanced Docking System (qtads) — shared/import libraries
#
# Fetched libraries (via FetchContent):
#   - fmt, spdlog — downloaded with SHA256 verification
#

# Configurable dependency versions (override via -D flags if needed)
set(GTEST_VERSION "1.14.0" CACHE STRING "GoogleTest version")
set(FMT_VERSION "10.1.1" CACHE STRING "fmt library version")
set(SPDLOG_VERSION "1.12.0" CACHE STRING "spdlog library version")

# Function to fetch all dependencies
function(fetch_project_dependencies)

    # =========================================================================
    # GoogleTest (pre-compiled local libraries)
    # =========================================================================
    set(GTEST_DIR "${CMAKE_SOURCE_DIR}/third_party/gtest")

    if(NOT EXISTS "${GTEST_DIR}/include/gtest/gtest.h")
        message(FATAL_ERROR "Pre-compiled GoogleTest not found at ${GTEST_DIR}. "
            "Please ensure third_party/gtest contains include/, lib/, and bin/ directories.")
    endif()

    message(STATUS "Using pre-compiled GoogleTest (static) from ${GTEST_DIR}")

    # GTest::gtest
    add_library(GTest::gtest STATIC IMPORTED GLOBAL)
    set_target_properties(GTest::gtest PROPERTIES
        IMPORTED_LOCATION_DEBUG           "${GTEST_DIR}/lib/Debug/gtest.lib"
        IMPORTED_LOCATION_RELEASE         "${GTEST_DIR}/lib/Release/gtest.lib"
        IMPORTED_LOCATION_RELWITHDEBINFO  "${GTEST_DIR}/lib/Release/gtest.lib"
        IMPORTED_LOCATION_MINSIZEREL      "${GTEST_DIR}/lib/Release/gtest.lib"
        INTERFACE_INCLUDE_DIRECTORIES     "${GTEST_DIR}/include"
    )

    # GTest::gtest_main
    add_library(GTest::gtest_main STATIC IMPORTED GLOBAL)
    set_target_properties(GTest::gtest_main PROPERTIES
        IMPORTED_LOCATION_DEBUG           "${GTEST_DIR}/lib/Debug/gtest_main.lib"
        IMPORTED_LOCATION_RELEASE         "${GTEST_DIR}/lib/Release/gtest_main.lib"
        IMPORTED_LOCATION_RELWITHDEBINFO  "${GTEST_DIR}/lib/Release/gtest_main.lib"
        IMPORTED_LOCATION_MINSIZEREL      "${GTEST_DIR}/lib/Release/gtest_main.lib"
        INTERFACE_INCLUDE_DIRECTORIES     "${GTEST_DIR}/include"
        INTERFACE_LINK_LIBRARIES          GTest::gtest
    )

    # GTest::gmock
    add_library(GTest::gmock STATIC IMPORTED GLOBAL)
    set_target_properties(GTest::gmock PROPERTIES
        IMPORTED_LOCATION_DEBUG           "${GTEST_DIR}/lib/Debug/gmock.lib"
        IMPORTED_LOCATION_RELEASE         "${GTEST_DIR}/lib/Release/gmock.lib"
        IMPORTED_LOCATION_RELWITHDEBINFO  "${GTEST_DIR}/lib/Release/gmock.lib"
        IMPORTED_LOCATION_MINSIZEREL      "${GTEST_DIR}/lib/Release/gmock.lib"
        INTERFACE_INCLUDE_DIRECTORIES     "${GTEST_DIR}/include"
        INTERFACE_LINK_LIBRARIES          GTest::gtest
    )

    # GTest::gmock_main
    add_library(GTest::gmock_main STATIC IMPORTED GLOBAL)
    set_target_properties(GTest::gmock_main PROPERTIES
        IMPORTED_LOCATION_DEBUG           "${GTEST_DIR}/lib/Debug/gmock_main.lib"
        IMPORTED_LOCATION_RELEASE         "${GTEST_DIR}/lib/Release/gmock_main.lib"
        IMPORTED_LOCATION_RELWITHDEBINFO  "${GTEST_DIR}/lib/Release/gmock_main.lib"
        IMPORTED_LOCATION_MINSIZEREL      "${GTEST_DIR}/lib/Release/gmock_main.lib"
        INTERFACE_INCLUDE_DIRECTORIES     "${GTEST_DIR}/include"
        INTERFACE_LINK_LIBRARIES          GTest::gmock
    )

    # =========================================================================
    # Qt Advanced Docking System (pre-compiled local libraries)
    # =========================================================================
    set(QTADS_DIR "${CMAKE_SOURCE_DIR}/third_party/qtads")

    if(NOT EXISTS "${QTADS_DIR}/include/DockManager.h")
        message(FATAL_ERROR "Pre-compiled QtADS not found at ${QTADS_DIR}. "
            "Please ensure third_party/qtads contains include/, lib/, and bin/ directories.")
    endif()

    message(STATUS "Using pre-compiled QtADS (static) from ${QTADS_DIR}")

    add_library(ads::qtadvanceddocking STATIC IMPORTED GLOBAL)
    set_target_properties(ads::qtadvanceddocking PROPERTIES
        IMPORTED_LOCATION_DEBUG           "${QTADS_DIR}/lib/Debug/qtadvanceddocking-qt6d_static.lib"
        IMPORTED_LOCATION_RELEASE         "${QTADS_DIR}/lib/Release/qtadvanceddocking-qt6_static.lib"
        IMPORTED_LOCATION_RELWITHDEBINFO  "${QTADS_DIR}/lib/Release/qtadvanceddocking-qt6_static.lib"
        IMPORTED_LOCATION_MINSIZEREL      "${QTADS_DIR}/lib/Release/qtadvanceddocking-qt6_static.lib"
        INTERFACE_INCLUDE_DIRECTORIES     "${QTADS_DIR}/include"
        INTERFACE_COMPILE_DEFINITIONS     ADS_STATIC
    )

    # =========================================================================
    # fmt v10.1.1
    # =========================================================================
    find_package(fmt 10.1.1 QUIET)
    if(NOT fmt_FOUND)
        message(STATUS "Fetching fmt v10.1.1...")
        FetchContent_Declare(
            fmt
            URL https://github.com/fmtlib/fmt/archive/refs/tags/10.1.1.zip
            URL_HASH SHA256=3c2e73019178ad72b0614a3124f25de454b9ca3a1afe81d5447b8d3cbdb6d322
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        # Don't install fmt headers/libs into the package
        set(FMT_INSTALL OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(fmt)
    else()
        message(STATUS "Found system fmt: ${fmt_VERSION}")
    endif()

    # =========================================================================
    # spdlog v1.12.0
    # =========================================================================
    find_package(spdlog 1.12.0 QUIET)
    if(NOT spdlog_FOUND)
        message(STATUS "Fetching spdlog v1.12.0...")
        FetchContent_Declare(
            spdlog
            URL https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.zip
            URL_HASH SHA256=6174bf8885287422a6c6a0312eb8a30e8d22bcfcee7c48a6d02d1835d7769232
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        # Don't install spdlog headers/libs into the package
        set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
        # Use the external fmt library (already fetched above) instead of
        # spdlog's bundled fmt v9 which triggers C4996 deprecation warnings
        # on MSVC 17.x (stdext::checked_array_iterator removal).
        set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(spdlog)
    else()
        message(STATUS "Found system spdlog: ${spdlog_VERSION}")
    endif()

endfunction()
