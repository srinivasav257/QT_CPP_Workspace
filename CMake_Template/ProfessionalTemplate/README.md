# Professional Qt CMake Template

A "Lifetime-Stable" industry-grade CMake template for Qt 6 C++ projects.

## Architecture Highlights

This template is designed to be **Structural Complete**: you never change the file structure, only add new modules.

-   **100% Target-Based**: No global state. Uses `target_link_libraries`, `target_compile_options`.
-   **Modular**: All logic (Warnings, Sanitizers, Dependencies) is isolated in `cmake/` modules.
-   **Scalable**: Works for a single `main.cpp` or a 50-module enterprise app.
-   **Upgradeable**: Change `CMAKE_CXX_STANDARD` in `StandardSettings.cmake` to move to C++20/26.
-   **Testing**: Dual support for **GoogleTest** and **QtTest**.

## Structure

```
├── cmake/
│   ├── CompilerWarnings.cmake   # Strict warnings (MSVC/Clang/GCC)
│   ├── FetchDependencies.cmake  # Centralized 3rd party lib fetching
│   ├── PreventInSourceBuilds.cmake
│   ├── Sanitizers.cmake         # ASan, TSan, UBSan, MSan (Toggleable)
│   ├── StandardSettings.cmake   # C++ Standard, LTO, Cache
│   └── StaticAnalyzers.cmake    # Clang-Tidy, CppCheck (Optional)
├── src/
│   ├── CMakeLists.txt           # Main targets
│   └── ...
├── tests/
│   ├── CMakeLists.txt           # GTest & QtTest configurations
│   ├── tst_gtest_main.cpp
│   └── tst_qtest_main.cpp
└── CMakeLists.txt               # minimal orchestration
```

## Usage

### 1. Build
```bash
cmake -S . -B build
cmake --build build
```

### 2. Run Tests
```bash
cd build
ctest --output-on-failure
```

### 3. Add Dependencies
Open `cmake/FetchDependencies.cmake` and add your `FetchContent_Declare`.

### 4. Enable Sanitizers
Sanitizers are OFF by default. Enable them at configuration time:
```bash
cmake -S . -B build -DENABLE_SANITIZER_ADDRESS=ON
```
Available options:
- `ENABLE_SANITIZER_ADDRESS`
- `ENABLE_SANITIZER_LEAK`
- `ENABLE_SANITIZER_UNDEFINED`
- `ENABLE_SANITIZER_THREAD`
- `ENABLE_SANITIZER_MEMORY` (Clang only)

### 5. Static Analysis
```bash
cmake -S . -B build -DENABLE_CLANG_TIDY=ON -DENABLE_CPPCHECK=ON
```
