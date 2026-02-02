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

### 🚀 Quick Start (Windows)
The easiest way to work with this project is using the included **`Build.bat`** script. It handles environment setup, building, testing, and packaging automatically.

1. Double-click `Build.bat`
2. Select an option:
   - **[1/2]**: Build Debug/Release
   - **[9]**: Create Installer (Package)

### Manual CLI
If you prefer standard CMake commands:

**Build:**
```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

**Test:**
```bash
cd build
ctest -C Release --output-on-failure
```

## Packaging & Deployment
This template supports one-click creation of **Installers (.exe)** and **Portable Zips**.

### Prerequisites
To generate the `.exe` installer, you need **NSIS**. Install it via Winget:
```powershell
winget install NSIS.NSIS
```

### Creating the Installer
1. Run `Build.bat`
2. Select **Option 9: Create Installer (Package)**
3. The script will:
   - Clean & Rebuild (Release)
   - Run `windeployqt` to copy necessary Qt DLLs
   - Run `CPack` to generate the installer
   - **Auto-Sign** the installer using a generated Self-Signed Certificate

**Output Location:** `build/QtTemplateProject-*-win64.exe` (and `.zip`)

### Code Signing
The project includes a helper script (`scripts/SignInstaller.ps1`) that:
- Automatically looks for `signtool.exe` (Windows SDK).
- Generates a **Self-Signed Certificate** (`QtTemplateDevCert.pfx`) if one is missing.
- Signs both the Application Binary (`QtTemplateApp.exe`) and the Installer (`setup.exe`).

> **Note:** Because the certificate is self-signed, Windows Defender/SmartScreen will show an "Unknown Publisher" warning. To remove this, replace the generated `.pfx` with a valid Code Signing Certificate bought from a generic CA.

## Configuration (.env)
The project supports dynamic environment configuration.
1. Create a `.env` file in the root.
2. Define variables like `API_URL=https://api.staging.com`.
3. These are compiled into `src/generated/env_config.h`.

## Advanced Features
**Sanitizers (ASan/Leak):**
```bash
cmake -S . -B build -DENABLE_SANITIZER_ADDRESS=ON
```

**Static Analysis:**
```bash
cmake -S . -B build -DENABLE_CLANG_TIDY=ON
```
