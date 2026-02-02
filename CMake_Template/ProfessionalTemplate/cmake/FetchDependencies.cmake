include(FetchContent)

# Function to fetch all dependencies
macro(fetch_project_dependencies)
    
    # --- GoogleTest ---
    find_package(GTest NAMES GTest googletest QUIET)
    if(NOT GTest_FOUND)
        FetchContent_Declare(
          googletest
          URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
          DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        # Prevent overriding parent project's compiler/linker settings on Windows
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    # --- fmt ---
    find_package(fmt 10.1.1 QUIET)
    if(NOT fmt_FOUND)
        FetchContent_Declare(
          fmt
          URL https://github.com/fmtlib/fmt/archive/refs/tags/10.1.1.zip
        )
        FetchContent_MakeAvailable(fmt)
    endif()

    # --- spdlog ---
    find_package(spdlog 1.12.0 QUIET)
    if(NOT spdlog_FOUND)
        FetchContent_Declare(
          spdlog
          URL https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.zip
        )
        FetchContent_MakeAvailable(spdlog)
    endif()

endmacro()
