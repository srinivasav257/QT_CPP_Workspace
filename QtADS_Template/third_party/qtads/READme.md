What You Will Build

third_party/
    qtads/
        include/
            ads/
        lib/
            Release/
                qtadvanceddocking-qt6.lib
            Debug/
                qtadvanceddocking-qt6d.lib
        bin/
            Release/qtadvanceddocking-qt6.dll
            Debug/qtadvanceddocking-qt6d.dll

✅ 1. Download QtADS Source (one-time only) - https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System
✅ 2. Build QtADS with CMake (VS/MSVC)
-> Create a folder Build
-> build\ - cmake -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=ON ..

✅ 3. after build is successful create libraries
cmake --build . --config Release
cmake --build . --config Debug

====================================================================================================


Setting up environment for Qt usage...
Remember to call vcvarsall.bat to complete environment setup!

C:\Qt\6.8.3\msvc2022_64>cd C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build>cmake -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=ON ..
-- The CXX compiler identification is MSVC 19.44.35217.0
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
-- Performing Test HAVE_STDATOMIC
-- Performing Test HAVE_STDATOMIC - Success
-- Found WrapAtomic: TRUE
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- The C compiler identification is MSVC 19.44.35217.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Configuring done (10.3s)
-- Generating done (1.1s)
-- Build files have been written to: C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/build

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build>cmake --build . --config Release
MSBuild version 17.14.23+b0019275e for .NET Framework

  1>Checking Build System
  Automatic MOC for target qtadvanceddocking-qt6
  1>Automatic RCC for ads.qrc
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/src/CMakeLists.txt
  ads_globals.cpp
  DockAreaTabBar.cpp
  DockAreaTitleBar.cpp
  DockAreaWidget.cpp
  DockContainerWidget.cpp
  DockManager.cpp
  DockOverlay.cpp
  DockSplitter.cpp
  DockWidget.cpp
  DockWidgetTab.cpp
  DockingStateReader.cpp
  DockFocusController.cpp
  ElidingLabel.cpp
  FloatingDockContainer.cpp
  FloatingDragPreview.cpp
  IconProvider.cpp
  DockComponentsFactory.cpp
  AutoHideSideBar.cpp
  AutoHideTab.cpp
  AutoHideDockContainer.cpp
  Generating Code...
  Compiling...
  PushButton.cpp
  ResizeHandle.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  qrc_ads.cpp
     Creating library C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/build/x64/lib/Release/qtadvanceddocking-qt6.lib and object C:/QT_Wor
  kspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/build/x64/lib/Release/qtadvanceddocking-qt6.exp
  qtadvanceddocking-qt6.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Release\qtadvanceddocking-qt6.dll
  Automatic MOC and UIC for target AdvancedDockingSystemDemo
  1>
  2>Automatic RCC for demo.qrc
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/demo/CMakeLists.txt
  main.cpp
  MainWindow.cpp
  StatusDialog.cpp
  ImageViewer.cpp
  RenderWidget.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  qrc_demo.cpp
  AdvancedDockingSystemDemo.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Release\AdvancedDockingSystemDemo.exe
  Automatic MOC and UIC for target AutoHideDragNDropExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/autohidedragndrop/CMakeLists.txt
  main.cpp
  mainwindow.cpp
  droppableitem.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  AutoHideDragNDropExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Release\AutoHideDragNDropExample.exe
  Automatic MOC and UIC for target AutoHideExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/autohide/CMakeLists.txt
  main.cpp
  mainwindow.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  AutoHideExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Release\AutoHideExample.exe
  Automatic MOC and UIC for target CentralWidgetExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/centralwidget/CMakeLists.txt
  main.cpp
  mainwindow.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  CentralWidgetExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Release\CentralWidgetExample.exe
  Automatic MOC for target DeleteOnCloseTest
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/deleteonclose/CMakeLists.txt
  main.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  DeleteOnCloseTest.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Release\DeleteOnCloseTest.exe
  Automatic MOC and UIC for target DockInDockExample
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/dockindock/CMakeLists.txt
  dockindock.cpp
  dockindockmanager.cpp
  perspectiveactions.cpp
  perspectives.cpp
  main.cpp
  mainframe.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  DockInDockExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Release\DockInDockExample.exe
  Automatic MOC and UIC for target EmptyDockAreaExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/emptydockarea/CMakeLists.txt
  main.cpp
  mainwindow.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  EmptyDockAreaExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Release\EmptyDockAreaExample.exe
  Automatic MOC and UIC for target HideShowExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/hideshow/CMakeLists.txt
  main.cpp
  MainWindow.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  HideShowExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Release\HideShowExample.exe
  Automatic MOC and UIC for target SidebarExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/sidebar/CMakeLists.txt
  main.cpp
  MainWindow.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  SidebarExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Release\SidebarExample.exe
  Automatic MOC and UIC for target SimpleExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/simple/CMakeLists.txt
  main.cpp
  MainWindow.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  SimpleExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Release\SimpleExample.exe
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/CMakeLists.txt

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build>cmake --build . --config Debug
MSBuild version 17.14.23+b0019275e for .NET Framework

  1>Checking Build System
  Automatic MOC for target qtadvanceddocking-qt6
  1>Automatic RCC for ads.qrc
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/src/CMakeLists.txt
  mocs_compilation_Debug.cpp
  ads_globals.cpp
  DockAreaTabBar.cpp
  DockAreaTitleBar.cpp
  DockAreaWidget.cpp
  DockContainerWidget.cpp
  DockManager.cpp
  DockOverlay.cpp
  DockSplitter.cpp
  DockWidget.cpp
  DockWidgetTab.cpp
  DockingStateReader.cpp
  DockFocusController.cpp
  ElidingLabel.cpp
  FloatingDockContainer.cpp
  FloatingDragPreview.cpp
  IconProvider.cpp
  DockComponentsFactory.cpp
  AutoHideSideBar.cpp
  AutoHideTab.cpp
  Generating Code...
  Compiling...
  AutoHideDockContainer.cpp
  PushButton.cpp
  ResizeHandle.cpp
  Generating Code...
  qrc_ads.cpp
     Creating library C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/build/x64/lib/Debug/qtadvanceddocking-qt6d.lib and object C:/QT_Work
  space/Qt_practice/Libraries/Qt-Advanced-Docking-System/build/x64/lib/Debug/qtadvanceddocking-qt6d.exp
  qtadvanceddocking-qt6.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Debug\qtadvanceddocking-qt6d.dll
  Automatic MOC and UIC for target AdvancedDockingSystemDemo
  1>
  2>Automatic RCC for demo.qrc
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/demo/CMakeLists.txt
  mocs_compilation_Debug.cpp
  main.cpp
  MainWindow.cpp
  StatusDialog.cpp
  ImageViewer.cpp
  RenderWidget.cpp
  Generating Code...
  qrc_demo.cpp
  AdvancedDockingSystemDemo.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Debug\AdvancedDockingSystemDemo.exe
  Automatic MOC and UIC for target AutoHideDragNDropExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/autohidedragndrop/CMakeLists.txt
  mocs_compilation_Debug.cpp
  main.cpp
  mainwindow.cpp
  droppableitem.cpp
  Generating Code...
  AutoHideDragNDropExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Debug\AutoHideDragNDropExample.exe
  Automatic MOC and UIC for target AutoHideExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/autohide/CMakeLists.txt
  mocs_compilation_Debug.cpp
  main.cpp
  mainwindow.cpp
  Generating Code...
  AutoHideExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Debug\AutoHideExample.exe
  Automatic MOC and UIC for target CentralWidgetExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/centralwidget/CMakeLists.txt
  mocs_compilation_Debug.cpp
  main.cpp
  mainwindow.cpp
  Generating Code...
  CentralWidgetExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Debug\CentralWidgetExample.exe
  Automatic MOC for target DeleteOnCloseTest
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/deleteonclose/CMakeLists.txt
  mocs_compilation_Debug.cpp
  main.cpp
  Generating Code...
  DeleteOnCloseTest.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Debug\DeleteOnCloseTest.exe
  Automatic MOC and UIC for target DockInDockExample
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/dockindock/CMakeLists.txt
  mocs_compilation_Debug.cpp
  dockindock.cpp
  dockindockmanager.cpp
  perspectiveactions.cpp
  perspectives.cpp
  main.cpp
  mainframe.cpp
  Generating Code...
  DockInDockExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Debug\DockInDockExample.exe
  Automatic MOC and UIC for target EmptyDockAreaExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/emptydockarea/CMakeLists.txt
  mocs_compilation_Debug.cpp
  main.cpp
  mainwindow.cpp
  Generating Code...
  EmptyDockAreaExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Debug\EmptyDockAreaExample.exe
  Automatic MOC and UIC for target HideShowExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/hideshow/CMakeLists.txt
  mocs_compilation_Debug.cpp
  main.cpp
  MainWindow.cpp
  Generating Code...
  HideShowExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Debug\HideShowExample.exe
  Automatic MOC and UIC for target SidebarExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/sidebar/CMakeLists.txt
  mocs_compilation_Debug.cpp
  main.cpp
  MainWindow.cpp
  Generating Code...
  SidebarExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Debug\SidebarExample.exe
  Automatic MOC and UIC for target SimpleExample
  1>
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/examples/simple/CMakeLists.txt
  mocs_compilation_Debug.cpp
  main.cpp
  MainWindow.cpp
  Generating Code...
  SimpleExample.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build\x64\bin\Debug\SimpleExample.exe
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/CMakeLists.txt

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build>

================================================== .a library ==========================================================

Setting up environment for Qt usage...
Remember to call vcvarsall.bat to complete environment setup!

C:\Qt\6.8.3\msvc2022_64>cd C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-debug

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-debug>cmake -G "Ninja" ^
More?   -DBUILD_EXAMPLES=OFF ^
More?   -DBUILD_STATIC=ON ^
More?   -DCMAKE_BUILD_TYPE=Debug ^
More?   -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64" ^
More?   -DCMAKE_INSTALL_PREFIX=../debug ^
More?   ..
-- The CXX compiler identification is GNU 14.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/msys64/mingw64/bin/c++.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE
-- Performing Test HAVE_STDATOMIC
-- Performing Test HAVE_STDATOMIC - Success
-- Found WrapAtomic: TRUE
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Configuring done (3.6s)
-- Generating done (0.2s)
-- Build files have been written to: C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/build-debug

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-debug>cmake --build .
[27/27] Linking CXX static library x64\lib\libqtadvanceddocking-qt6d_static.a

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-debug>cd ..

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System>cd build-release

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-release>cmake -G "Ninja" ^
More?   -DBUILD_EXAMPLES=OFF ^
More?   -DBUILD_STATIC=ON ^
More?   -DCMAKE_BUILD_TYPE=Release ^
More?   -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64" ^
More?   -DCMAKE_INSTALL_PREFIX=../release ^
More?   ..
-- The CXX compiler identification is GNU 14.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/msys64/mingw64/bin/c++.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE
-- Performing Test HAVE_STDATOMIC
-- Performing Test HAVE_STDATOMIC - Success
-- Found WrapAtomic: TRUE
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Configuring done (4.5s)
-- Generating done (0.2s)
-- Build files have been written to: C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/build-release

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-release>cmake --build .
[27/27] Linking CXX static library x64\lib\libqtadvanceddocking-qt6_static.a

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-release>


================================================== .lib library ==========================================================



Setting up environment for Qt usage...
Remember to call vcvarsall.bat to complete environment setup!

C:\Qt\6.8.3\msvc2022_64>cd C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-debug

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-debug>cmake -G "Visual Studio 17 2022" -A x64 ^
More?   -DBUILD_STATIC=ON ^
More?   -DBUILD_EXAMPLES=OFF ^
More?   -DCMAKE_BUILD_TYPE=Debug ^
More?   -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64" ^
More?   ..
-- The CXX compiler identification is MSVC 19.44.35217.0
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
-- Performing Test HAVE_STDATOMIC
-- Performing Test HAVE_STDATOMIC - Success
-- Found WrapAtomic: TRUE
-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
-- Configuring done (6.6s)
-- Generating done (0.3s)
CMake Warning:
  Manually-specified variables were not used by the project:

    CMAKE_BUILD_TYPE


-- Build files have been written to: C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/build-debug

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-debug>cmake --build . --config Debug
MSBuild version 17.14.23+b0019275e for .NET Framework

  1>Checking Build System
  Automatic MOC for target qtadvanceddocking-qt6
  1>Automatic RCC for ads.qrc
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/src/CMakeLists.txt
  mocs_compilation_Debug.cpp
  ads_globals.cpp
  DockAreaTabBar.cpp
  DockAreaTitleBar.cpp
  DockAreaWidget.cpp
  DockContainerWidget.cpp
  DockManager.cpp
  DockOverlay.cpp
  DockSplitter.cpp
  DockWidget.cpp
  DockWidgetTab.cpp
  DockingStateReader.cpp
  DockFocusController.cpp
  ElidingLabel.cpp
  FloatingDockContainer.cpp
  FloatingDragPreview.cpp
  IconProvider.cpp
  DockComponentsFactory.cpp
  AutoHideSideBar.cpp
  AutoHideTab.cpp
  Generating Code...
  Compiling...
  AutoHideDockContainer.cpp
  PushButton.cpp
  ResizeHandle.cpp
  Generating Code...
  qrc_ads.cpp
  qtadvanceddocking-qt6.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-debug\x64\lib\Debug\qtadvanceddocking-qt6d_static
  .lib
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/CMakeLists.txt

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-debug>


-- Build files have been written to: C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/build-release

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-release>cmake --build . --config Release
MSBuild version 17.14.23+b0019275e for .NET Framework

  1>Checking Build System
  Automatic MOC for target qtadvanceddocking-qt6
  1>Automatic RCC for ads.qrc
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/src/CMakeLists.txt
  ads_globals.cpp
  DockAreaTabBar.cpp
  DockAreaTitleBar.cpp
  DockAreaWidget.cpp
  DockContainerWidget.cpp
  DockManager.cpp
  DockOverlay.cpp
  DockSplitter.cpp
  DockWidget.cpp
  DockWidgetTab.cpp
  DockingStateReader.cpp
  DockFocusController.cpp
  ElidingLabel.cpp
  FloatingDockContainer.cpp
  FloatingDragPreview.cpp
  IconProvider.cpp
  DockComponentsFactory.cpp
  AutoHideSideBar.cpp
  AutoHideTab.cpp
  AutoHideDockContainer.cpp
  Generating Code...
  Compiling...
  PushButton.cpp
  ResizeHandle.cpp
  mocs_compilation_Release.cpp
  Generating Code...
  qrc_ads.cpp
  qtadvanceddocking-qt6.vcxproj -> C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-release\x64\lib\Release\qtadvanceddocking-qt6_sta
  tic.lib
  Building Custom Rule C:/QT_Workspace/Qt_practice/Libraries/Qt-Advanced-Docking-System/CMakeLists.txt

C:\QT_Workspace\Qt_practice\Libraries\Qt-Advanced-Docking-System\build-release>


