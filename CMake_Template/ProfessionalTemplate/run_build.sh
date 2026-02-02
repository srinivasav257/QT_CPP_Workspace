#!/usr/bin/env bash
#
# Professional Qt CMake Build Tool - Linux/macOS
# Mirrors the functionality of Run_Build.bat for cross-platform support
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Detect platform
if [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macOS"
    GENERATOR="Ninja"
    # Check if Ninja is available, fallback to Makefiles
    if ! command -v ninja &> /dev/null; then
        GENERATOR="Unix Makefiles"
    fi
else
    PLATFORM="Linux"
    GENERATOR="Ninja"
    if ! command -v ninja &> /dev/null; then
        GENERATOR="Unix Makefiles"
    fi
fi

# Parallel jobs
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

print_header() {
    echo -e "${BLUE}==========================================================${NC}"
    echo -e "${BLUE}     Professional Qt CMake Build Tool (${PLATFORM})${NC}"
    echo -e "${BLUE}==========================================================${NC}"
    echo ""
}

print_menu() {
    echo "   --- Standard Incremental Builds ---"
    echo "   [1] Build Debug"
    echo "   [2] Build Release"
    echo ""
    echo "   --- Clean Rebuilds (Deletes build folder) ---"
    echo "   [3] Clean & Build Debug"
    echo "   [4] Clean & Build Release"
    echo ""
    echo "   --- Advanced Modes ---"
    echo "   [5] SuperBuild (Build Dependencies + Project)"
    echo "   [6] Build with Sanitizers (Address + Leak)"
    echo "   [7] Run Unit Tests (CTest)"
    echo "   [8] Build with Coverage"
    echo "   [9] Create Package (CPack)"
    echo ""
    echo "   --- CMake Presets (Modern) ---"
    echo "   [p] List available presets"
    echo "   [w] Run workflow preset"
    echo ""
    echo "   [q] Quit"
    echo ""
}

info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

build_debug() {
    info "Building Debug (Incremental)..."
    cmake -S . -B build/debug -G "$GENERATOR" -DCMAKE_BUILD_TYPE=Debug
    cmake --build build/debug -j "$JOBS"
    info "Build complete. Binary at: build/debug/"
}

build_release() {
    info "Building Release (Incremental)..."
    cmake -S . -B build/release -G "$GENERATOR" -DCMAKE_BUILD_TYPE=Release
    cmake --build build/release -j "$JOBS"
    info "Build complete. Binary at: build/release/"
}

clean_build_debug() {
    warn "Deleting build/debug directory..."
    rm -rf build/debug
    info "Configuring Debug..."
    cmake -S . -B build/debug -G "$GENERATOR" -DCMAKE_BUILD_TYPE=Debug
    info "Building..."
    cmake --build build/debug -j "$JOBS"
    info "Clean build complete."
}

clean_build_release() {
    warn "Deleting build/release directory..."
    rm -rf build/release
    info "Configuring Release..."
    cmake -S . -B build/release -G "$GENERATOR" -DCMAKE_BUILD_TYPE=Release
    info "Building..."
    cmake --build build/release -j "$JOBS"
    info "Clean build complete."
}

superbuild() {
    warn "Deleting build_superbuild directory..."
    rm -rf build_superbuild
    info "Configuring SuperBuild..."
    cmake -S . -B build_superbuild -G "$GENERATOR" -DENABLE_SUPERBUILD=ON -DCMAKE_BUILD_TYPE=Release
    info "Building Dependencies & Project..."
    cmake --build build_superbuild -j "$JOBS"
    info "SuperBuild complete."
}

build_sanitizers() {
    warn "Cleaning build for Sanitizer run..."
    rm -rf build/sanitizers
    info "Configuring with ASan + LSan..."
    cmake -S . -B build/sanitizers -G "$GENERATOR" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DENABLE_SANITIZER_ADDRESS=ON \
        -DENABLE_SANITIZER_LEAK=ON
    info "Building..."
    cmake --build build/sanitizers -j "$JOBS"
    info "Sanitizer build complete. Run your binary to check for issues."
}

run_tests() {
    if [[ -d "build/debug" ]]; then
        BUILD_DIR="build/debug"
    elif [[ -d "build/release" ]]; then
        BUILD_DIR="build/release"
    else
        error "Build folder not found. Please build first."
        return 1
    fi

    info "Running tests in $BUILD_DIR..."
    cd "$BUILD_DIR"
    ctest --output-on-failure
    cd "$SCRIPT_DIR"
    info "Tests complete."
}

build_coverage() {
    warn "Setting up coverage build..."
    rm -rf build/coverage
    info "Configuring with Coverage..."
    cmake -S . -B build/coverage -G "$GENERATOR" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DENABLE_COVERAGE=ON
    info "Building..."
    cmake --build build/coverage -j "$JOBS"
    info "Coverage build complete."
    info "Run tests, then use 'cmake --build build/coverage --target coverage' to generate report."
}

create_package() {
    info "Cleaning build directory for fresh release build..."
    rm -rf build/package
    info "Configuring Release..."
    cmake -S . -B build/package -G "$GENERATOR" -DCMAKE_BUILD_TYPE=Release
    info "Building Release..."
    cmake --build build/package -j "$JOBS"
    info "Packaging (CPack)..."
    cd build/package
    cpack
    cd "$SCRIPT_DIR"
    info "Package created in build/package/"
}

list_presets() {
    info "Available CMake Presets:"
    echo ""
    cmake --list-presets
    echo ""
    info "Configure presets:"
    cmake --list-presets=configure 2>/dev/null || true
    echo ""
    info "Build presets:"
    cmake --list-presets=build 2>/dev/null || true
    echo ""
    info "Test presets:"
    cmake --list-presets=test 2>/dev/null || true
    echo ""
    info "Workflow presets:"
    cmake --list-presets=workflow 2>/dev/null || true
}

run_workflow() {
    echo "Available workflow presets:"
    cmake --list-presets=workflow 2>/dev/null || echo "  (none found)"
    echo ""
    read -rp "Enter workflow preset name (e.g., dev, release, ci): " workflow
    if [[ -n "$workflow" ]]; then
        info "Running workflow: $workflow"
        cmake --workflow --preset="$workflow"
    fi
}

# Main menu loop
main() {
    while true; do
        print_header
        print_menu

        read -rp "Select option: " choice
        echo ""

        case $choice in
            1) build_debug ;;
            2) build_release ;;
            3) clean_build_debug ;;
            4) clean_build_release ;;
            5) superbuild ;;
            6) build_sanitizers ;;
            7) run_tests ;;
            8) build_coverage ;;
            9) create_package ;;
            p|P) list_presets ;;
            w|W) run_workflow ;;
            q|Q)
                info "Goodbye!"
                exit 0
                ;;
            *)
                error "Invalid option: $choice"
                ;;
        esac

        echo ""
        read -rp "Press Enter to continue..."
    done
}

# Allow running specific commands directly
if [[ $# -gt 0 ]]; then
    case $1 in
        debug) build_debug ;;
        release) build_release ;;
        clean-debug) clean_build_debug ;;
        clean-release) clean_build_release ;;
        superbuild) superbuild ;;
        sanitizers) build_sanitizers ;;
        test) run_tests ;;
        coverage) build_coverage ;;
        package) create_package ;;
        presets) list_presets ;;
        *)
            echo "Usage: $0 [debug|release|clean-debug|clean-release|superbuild|sanitizers|test|coverage|package|presets]"
            echo "Or run without arguments for interactive menu."
            exit 1
            ;;
    esac
else
    main
fi
