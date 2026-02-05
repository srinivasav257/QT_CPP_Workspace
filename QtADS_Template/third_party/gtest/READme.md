What You Will Build

You will end up with:

gtest.lib / gtestd.lib (static debug & release)
gmock.lib / gmockd.lib
gtest.dll, gmock.dll (optional)

✅ 1. Download GoogleTest Source (one-time only) - https://github.com/google/googletest/releases
✅ 2. Build GoogleTest with CMake (VS/MSVC)
-> Create a folder Build
-> build\ - cmake -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=ON ..

✅ 3. after build is successful create libraries
cmake --build . --config Release
cmake --build . --config Debug

====================================================================================================


C:\QT_Workspace\Qt_practice\Libraries\googletest-main>build
'build' is not recognized as an internal or external command,
operable program or batch file.

C:\QT_Workspace\Qt_practice\Libraries\googletest-main>cd buid
The system cannot find the path specified.

C:\QT_Workspace\Qt_practice\Libraries\googletest-main>cd build

C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build>cmake -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=ON ..
-- The C compiler identification is MSVC 19.44.35217.0
-- The CXX compiler identification is MSVC 19.44.35217.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Failed
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - not found
-- Found Threads: TRUE
-- Configuring done (5.7s)
-- Generating done (0.1s)
-- Build files have been written to: C:/QT_Workspace/Qt_practice/Libraries/googletest-main/build

C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build>cmake --build . --config Release
MSBuild version 17.14.23+b0019275e for .NET Framework

  1>Checking Build System
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googlemock/CMakeLists.txt
  gtest-all.cc
  gmock-all.cc
  Generating Code...
     Creating library C:/QT_Workspace/Qt_practice/Libraries/googletest-main/build/lib/Release/gmock.lib and object C:/QT_Workspace/Qt_practice/Libraries/go
  ogletest-main/build/lib/Release/gmock.exp
  gmock.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build\bin\Release\gmock.dll
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googlemock/CMakeLists.txt
  gtest-all.cc
  gmock-all.cc
  gmock_main.cc
  Generating Code...
     Creating library C:/QT_Workspace/Qt_practice/Libraries/googletest-main/build/lib/Release/gmock_main.lib and object C:/QT_Workspace/Qt_practice/Librari
  es/googletest-main/build/lib/Release/gmock_main.exp
  gmock_main.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build\bin\Release\gmock_main.dll
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googletest/CMakeLists.txt
  gtest-all.cc
     Creating library C:/QT_Workspace/Qt_practice/Libraries/googletest-main/build/lib/Release/gtest.lib and object C:/QT_Workspace/Qt_practice/Libraries/go
  ogletest-main/build/lib/Release/gtest.exp
  gtest.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build\bin\Release\gtest.dll
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googletest/CMakeLists.txt
  gtest_main.cc
     Creating library C:/QT_Workspace/Qt_practice/Libraries/googletest-main/build/lib/Release/gtest_main.lib and object C:/QT_Workspace/Qt_practice/Librari
  es/googletest-main/build/lib/Release/gtest_main.exp
  gtest_main.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build\bin\Release\gtest_main.dll
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/CMakeLists.txt

C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build>cmake --build . --config Debug
MSBuild version 17.14.23+b0019275e for .NET Framework

  1>Checking Build System
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googlemock/CMakeLists.txt
  gtest-all.cc
  gmock-all.cc
  Generating Code...
     Creating library C:/QT_Workspace/Qt_practice/Libraries/googletest-main/build/lib/Debug/gmock.lib and object C:/QT_Workspace/Qt_practice/Libraries/goog
  letest-main/build/lib/Debug/gmock.exp
  gmock.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build\bin\Debug\gmock.dll
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googlemock/CMakeLists.txt
  gtest-all.cc
  gmock-all.cc
  gmock_main.cc
  Generating Code...
     Creating library C:/QT_Workspace/Qt_practice/Libraries/googletest-main/build/lib/Debug/gmock_main.lib and object C:/QT_Workspace/Qt_practice/Libraries
  /googletest-main/build/lib/Debug/gmock_main.exp
  gmock_main.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build\bin\Debug\gmock_main.dll
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googletest/CMakeLists.txt
  gtest-all.cc
     Creating library C:/QT_Workspace/Qt_practice/Libraries/googletest-main/build/lib/Debug/gtest.lib and object C:/QT_Workspace/Qt_practice/Libraries/goog
  letest-main/build/lib/Debug/gtest.exp
  gtest.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build\bin\Debug\gtest.dll
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googletest/CMakeLists.txt
  gtest_main.cc
     Creating library C:/QT_Workspace/Qt_practice/Libraries/googletest-main/build/lib/Debug/gtest_main.lib and object C:/QT_Workspace/Qt_practice/Libraries
  /googletest-main/build/lib/Debug/gtest_main.exp
  gtest_main.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build\bin\Debug\gtest_main.dll
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/CMakeLists.txt

C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build>


====================================================== static =========================================================
🟦 1. Static Debug build

Create debug build directory - build-debug
Configure for static Debug:
cmake -G "Visual Studio 17 2022" -A x64 ^
  -DBUILD_SHARED_LIBS=OFF ^
  -DCMAKE_BUILD_TYPE=Debug ^
  ..
buidl: cmake --build . --config Debug

🟩 2. Static Release build

Create debug build directory - build-release
Configure for static Debug:
cmake -G "Visual Studio 17 2022" -A x64 ^
  -DBUILD_SHARED_LIBS=OFF ^
  -DCMAKE_BUILD_TYPE=Release ^
  ..
buidl: cmake --build . --config Release



Setting up environment for Qt usage...
Remember to call vcvarsall.bat to complete environment setup!

C:\Qt\6.8.3\msvc2022_64>cd C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-debug

C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-debug>cmake -G "Visual Studio 17 2022" -A x64 ^
More?   -DBUILD_SHARED_LIBS=OFF ^
More?   -DCMAKE_BUILD_TYPE=Debug ^
More?   ..
-- The C compiler identification is MSVC 19.44.35217.0
-- The CXX compiler identification is MSVC 19.44.35217.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Failed
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - not found
-- Found Threads: TRUE
-- Configuring done (6.3s)
-- Generating done (0.1s)
CMake Warning:
  Manually-specified variables were not used by the project:

    CMAKE_BUILD_TYPE


-- Build files have been written to: C:/QT_Workspace/Qt_practice/Libraries/googletest-main/build-debug

C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-debug>cmake --build . --config Debug
MSBuild version 17.14.23+b0019275e for .NET Framework

  1>Checking Build System
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googlemock/CMakeLists.txt
  gtest-all.cc
  gmock-all.cc
  Generating Code...
  gmock.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-debug\lib\Debug\gmock.lib
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googlemock/CMakeLists.txt
  gtest-all.cc
  gmock-all.cc
  gmock_main.cc
  Generating Code...
  gmock_main.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-debug\lib\Debug\gmock_main.lib
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googletest/CMakeLists.txt
  gtest-all.cc
  gtest.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-debug\lib\Debug\gtest.lib
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googletest/CMakeLists.txt
  gtest_main.cc
  gtest_main.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-debug\lib\Debug\gtest_main.lib
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/CMakeLists.txt

C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-debug>cd ..

C:\QT_Workspace\Qt_practice\Libraries\googletest-main>cd build-release

C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-release>cmake -G "Visual Studio 17 2022" -A x64 ^
More?   -DBUILD_SHARED_LIBS=OFF ^
More?   -DCMAKE_BUILD_TYPE=Release ^
More?   ..
-- The C compiler identification is MSVC 19.44.35217.0
-- The CXX compiler identification is MSVC 19.44.35217.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Failed
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - not found
-- Found Threads: TRUE
-- Configuring done (6.0s)
-- Generating done (0.1s)
CMake Warning:
  Manually-specified variables were not used by the project:

    CMAKE_BUILD_TYPE


-- Build files have been written to: C:/QT_Workspace/Qt_practice/Libraries/googletest-main/build-release

C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-release>cmake --build . --config Release
MSBuild version 17.14.23+b0019275e for .NET Framework

  1>Checking Build System
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googlemock/CMakeLists.txt
  gtest-all.cc
  gmock-all.cc
  Generating Code...
  gmock.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-release\lib\Release\gmock.lib
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googlemock/CMakeLists.txt
  gtest-all.cc
  gmock-all.cc
  gmock_main.cc
  Generating Code...
  gmock_main.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-release\lib\Release\gmock_main.lib
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googletest/CMakeLists.txt
  gtest-all.cc
  gtest.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-release\lib\Release\gtest.lib
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/googletest/CMakeLists.txt
  gtest_main.cc
  gtest_main.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-release\lib\Release\gtest_main.lib
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/googletest-main/CMakeLists.txt

C:\QT_Workspace\Qt_practice\Libraries\googletest-main\build-release>