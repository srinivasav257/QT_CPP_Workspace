cmake_minimum_required(VERSION 3.21)  # Match CMakePresets.json requirement

# --- Package Manager Integration ---
# Must be included BEFORE project() to handle Toolchain setup (Vcpkg/Conan)
include(cmake/PackageManager.cmake)

# --- Project Meta ---
project(QtTemplateProject VERSION 1.0.0 LANGUAGES CXX)

# --- Options ---
option(ENABLE_SUPERBUILD "Enable SuperBuild mode (build deps then project)" OFF)

if(ENABLE_SUPERBUILD)
    # In SuperBuild mode, we only process the SuperBuild script.
    # The script will re-invoke this CMakeLists.txt with ENABLE_SUPERBUILD=OFF
    include(cmake/SuperBuild.cmake)
    return() 
endif()

# --- Module Path ---
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# --- Includes ---
include(PreventInSourceBuilds)
include(StandardSettings)
include(CompilerWarnings)
include(Sanitizers)
include(StaticAnalyzers)
include(Coverage)
include(FetchDependencies)
include(AutoVersion)
include(PrecompiledHeaders)
include(Documentation)
include(CodeFormatTargets)
include(QmlSupport)
include(Deployment)
include(InstallRules)
include(EnvConfig)

# --- Environment ---
load_dotenv()
add_config_var(API_URL STRING "https://localhost:8080" "Base API Endpoint")
add_config_var(API_TIMEOUT_MS INT 5000 "Network timeout in ms")
add_config_var(ENABLE_DEBUG_MENU BOOL ON "Enable debug menu")

# --- Dependencies ---
# Qt 6.2+ required for modern features (Qt Quick Compiler, better CMake support)
find_package(Qt6 6.2 COMPONENTS Core Gui Widgets Test REQUIRED)
message(STATUS "Found Qt ${Qt6_VERSION}")

# Global Qt Integration
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Fetch 3rd party libs (e.g. GTest)
fetch_project_dependencies()

# --- Defines ---
# Global definitions if needed (prefer target_compile_definitions)
# add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x060000)

# --- Subdirectories ---
add_subdirectory(src)
add_subdirectory(tests)

# --- Packaging ---
include(Packaging)
setup_packaging()
