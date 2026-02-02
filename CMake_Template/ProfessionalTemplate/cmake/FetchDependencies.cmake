include(FetchContent)

# FetchDependencies.cmake
# Fetches third-party dependencies with SHA256 verification for security.
# Uses find_package fallback to prefer system-installed packages when available.
#
# SHA256 hashes ensure:
#   1. Integrity: Downloaded files haven't been corrupted
#   2. Security: Protection against supply-chain attacks
#   3. Reproducibility: Same source every time
#
# To update a dependency:
#   1. Change the URL/version
#   2. Download the new archive and compute: shasum -a 256 <file>
#   3. Update the URL_HASH below
#

# Function to fetch all dependencies
macro(fetch_project_dependencies)

    # --- GoogleTest v1.14.0 ---
    # https://github.com/google/googletest/releases
    find_package(GTest NAMES GTest googletest QUIET)
    if(NOT GTest_FOUND)
        message(STATUS "Fetching GoogleTest v1.14.0...")
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
            URL_HASH SHA256=1f357c27ca988c3f7c6b4bf68a9395005ac6761f034046e9dde0896e3aba00e4
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        # Prevent overriding parent project's compiler/linker settings on Windows
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    else()
        message(STATUS "Found system GoogleTest: ${GTest_VERSION}")
    endif()

    # --- fmt v10.1.1 ---
    # https://github.com/fmtlib/fmt/releases
    # Modern C++ formatting library (std::format predecessor)
    find_package(fmt 10.1.1 QUIET)
    if(NOT fmt_FOUND)
        message(STATUS "Fetching fmt v10.1.1...")
        FetchContent_Declare(
            fmt
            URL https://github.com/fmtlib/fmt/archive/refs/tags/10.1.1.zip
            URL_HASH SHA256=3c2e73019178ad72b0614a3124f25de454b9ca3a1afe81d5447b8d3cbdb6d322
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        FetchContent_MakeAvailable(fmt)
    else()
        message(STATUS "Found system fmt: ${fmt_VERSION}")
    endif()

    # --- spdlog v1.12.0 ---
    # https://github.com/gabime/spdlog/releases
    # Fast C++ logging library
    find_package(spdlog 1.12.0 QUIET)
    if(NOT spdlog_FOUND)
        message(STATUS "Fetching spdlog v1.12.0...")
        FetchContent_Declare(
            spdlog
            URL https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.zip
            URL_HASH SHA256=6174bf8885287422a6c6a0312eb8a30e8d22bcfcee7c48a6d02d1835d7769232
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        FetchContent_MakeAvailable(spdlog)
    else()
        message(STATUS "Found system spdlog: ${spdlog_VERSION}")
    endif()

endmacro()
