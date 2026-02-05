cmake_minimum_required(VERSION 3.16)
project(ads_demo VERSION 1.0.0)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.5 COMPONENTS Core Gui Widgets Quick QuickWidgets REQUIRED)
if(WIN32 AND QT_VERSION_MAJOR LESS 6)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS AxContainer REQUIRED)
endif()
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(ads_demo_SRCS
    main.cpp
    MainWindow.cpp
    mainwindow.ui
    StatusDialog.cpp
    StatusDialog.ui
    ImageViewer.cpp
    RenderWidget.cpp
	demo.qrc
)
add_executable(AdvancedDockingSystemDemo WIN32 ${ads_demo_SRCS})
# QtADS pre-built library (static)
set(QTADS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qtads")

add_library(qtadvanceddocking STATIC IMPORTED)
set_target_properties(qtadvanceddocking PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${QTADS_DIR}/include"
    IMPORTED_LOCATION_RELEASE "${QTADS_DIR}/lib/Release/qtadvanceddocking-qt6_static.lib"
    IMPORTED_LOCATION_DEBUG "${QTADS_DIR}/lib/Debug/qtadvanceddocking-qt6d_static.lib"
)

target_include_directories(AdvancedDockingSystemDemo PRIVATE "${QTADS_DIR}/include")
target_compile_definitions(AdvancedDockingSystemDemo PRIVATE ADS_STATIC)
target_link_libraries(AdvancedDockingSystemDemo PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickWidgets
    qtadvanceddocking
)
if(WIN32 AND QT_VERSION_MAJOR LESS 6)
    target_link_libraries(AdvancedDockingSystemDemo PUBLIC Qt${QT_VERSION_MAJOR}::AxContainer)
endif()
set_target_properties(AdvancedDockingSystemDemo PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION 1.0.0
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

