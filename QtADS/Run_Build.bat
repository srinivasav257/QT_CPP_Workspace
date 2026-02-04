@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

:: Define MSVC Generator
set CMAKE_GEN=-G "Visual Studio 17 2022" -A x64

:MENU
cls
echo ==========================================================
echo      Professional Qt CMake Build Tool
echo ==========================================================
echo.
echo   --- Standard Incremental Builds ---
echo   [1] Build Debug
echo   [2] Build Release
echo.
echo   --- Clean Rebuilds (Deletes build folder) ---
echo   [3] Clean ^& Build Debug
echo   [4] Clean ^& Build Release
echo.
echo   --- Advanced Modes ---
echo   [5] SuperBuild (Build Dependencies + Project)
echo   [6] Build with Sanitizers (Address + Leak)
echo   [7] Run Unit Tests (CTest)
echo   [8] Generate VS Solution ^& Open
echo   [9] Create Installer (Package)
echo.
echo   [Q] Quit
echo.
set /p choice="Select option: "

if "%choice%"=="1" goto BUILD_DEBUG
if "%choice%"=="2" goto BUILD_RELEASE
if "%choice%"=="3" goto CLEAN_DEBUG
if "%choice%"=="4" goto CLEAN_RELEASE
if "%choice%"=="5" goto SUPERBUILD
if "%choice%"=="6" goto SANITIZERS
if "%choice%"=="7" goto RUN_TESTS
if "%choice%"=="8" goto GEN_VS
if "%choice%"=="9" goto PACKAGE
if /i "%choice%"=="q" goto EOF

goto MENU

:: ---------------------------------------------------------------------------
:BUILD_DEBUG
echo.
echo [INFO] Building Debug (Incremental)...
cmake -S . -B build %CMAKE_GEN% -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 pause && goto EOF
cmake --build build --config Debug
if %errorlevel% neq 0 pause && goto EOF
echo [INFO] Launching QtTemplateApp (Debug)...
start "" "build\src\Debug\QtTemplateApp.exe"
pause
goto EOF

:: ---------------------------------------------------------------------------
:BUILD_RELEASE
echo.
echo [INFO] Building Release (Incremental)...
cmake -S . -B build %CMAKE_GEN% -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 pause && goto EOF
cmake --build build --config Release
if %errorlevel% neq 0 pause && goto EOF
echo [INFO] Launching QtTemplateApp (Release)...
start "" "build\src\Release\QtTemplateApp.exe"
pause
goto EOF

:: ---------------------------------------------------------------------------
:CLEAN_DEBUG
echo.
echo [WARN] Deleting build directory...
if exist build rmdir /s /q build
echo [INFO] Configuring Debug...
cmake -S . -B build %CMAKE_GEN% -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 pause && goto EOF
echo [INFO] Building...
cmake --build build --config Debug
pause
goto EOF

:: ---------------------------------------------------------------------------
:CLEAN_RELEASE
echo.
echo [WARN] Deleting build directory...
if exist build rmdir /s /q build
echo [INFO] Configuring Release...
cmake -S . -B build %CMAKE_GEN% -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 pause && goto EOF
echo [INFO] Building...
cmake --build build --config Release
pause
goto EOF

:: ---------------------------------------------------------------------------
:SUPERBUILD
echo.
echo [WARN] Deleting build_sb directory...
if exist build_sb rmdir /s /q build_sb
echo [INFO] Configuring SuperBuild...
cmake -S . -B build_sb %CMAKE_GEN% -DENABLE_SUPERBUILD=ON -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 pause && goto EOF
echo [INFO] Building Dependencies ^& Project...
cmake --build build_sb --config Release
pause
goto EOF

:: ---------------------------------------------------------------------------
:SANITIZERS
echo.
echo [WARN] Cleaning build for Sanitizer run...
if exist build rmdir /s /q build
echo [INFO] Configuring with ASan...
cmake -S . -B build %CMAKE_GEN% -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZER_ADDRESS=ON -DENABLE_SANITIZER_LEAK=ON
if %errorlevel% neq 0 pause && goto EOF
echo [INFO] Building...
cmake --build build --config Debug
pause
goto EOF

:: ---------------------------------------------------------------------------
:RUN_TESTS
echo.
if not exist build (
    echo [ERROR] Build folder not found. Please build first.
    pause
    goto EOF
)
cd build
ctest --output-on-failure -C Debug
cd ..
pause
goto EOF

:: ---------------------------------------------------------------------------
:GEN_VS
echo.
if exist build_vs rmdir /s /q build_vs
mkdir build_vs
cd build_vs
echo [INFO] Generating Visual Studio Solution...
cmake .. %CMAKE_GEN%
if %errorlevel% neq 0 pause && cd .. && goto EOF
echo [INFO] Opening Solution...
start QtTemplateProject.sln
cd ..
goto EOF

:: ---------------------------------------------------------------------------
:PACKAGE
echo.
echo [INFO] Cleaning build directory for fresh release build...
if exist build rmdir /s /q build
echo [INFO] Configuring Release...
cmake -S . -B build %CMAKE_GEN% -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 pause && goto EOF
echo [INFO] Building Release...
cmake --build build --config Release
if %errorlevel% neq 0 pause && goto EOF

:: Sign the Application Executable (so the ZIP version is also signed)
echo [INFO] Signing Application Binary...
if exist "build\src\Release\QtTemplateApp.exe" (
    powershell -NoProfile -ExecutionPolicy Bypass -File "scripts\SignInstaller.ps1" -TargetFile "build\src\Release\QtTemplateApp.exe"
)

echo [INFO] Packaging (CPack)...
cd build
cpack -C Release
if %errorlevel% neq 0 pause && goto EOF

:: Sign the Installer (The setup.exe wrapper)
echo [INFO] Signing Installer...
for %%f in (QtTemplateProject-*-win64.exe) do (
    powershell -NoProfile -ExecutionPolicy Bypass -File "..\scripts\SignInstaller.ps1" -TargetFile "%%f"
)

echo [INFO] Installer signed successfully.
cd ..
pause
goto EOF

:EOF
exit /b
