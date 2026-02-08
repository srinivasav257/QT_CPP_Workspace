cmake_minimum_required(VERSION 3.16)

project(ui_design_QtADS_Template VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
add_subdirectory(lib/ide_shell)

set(QTADS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qtads")
set(QTADS_LIB_DEBUG "${QTADS_DIR}/lib/Debug/qtadvanceddocking-qt6d_static.lib")
set(QTADS_LIB_RELEASE "${QTADS_DIR}/lib/Release/qtadvanceddocking-qt6_static.lib")

if(NOT EXISTS "${QTADS_DIR}/include/DockManager.h")
    message(FATAL_ERROR "QtADS headers not found at ${QTADS_DIR}/include")
endif()

if(NOT EXISTS "${QTADS_LIB_DEBUG}" OR NOT EXISTS "${QTADS_LIB_RELEASE}")
    message(FATAL_ERROR "QtADS static libraries not found. Expected:\n  ${QTADS_LIB_DEBUG}\n  ${QTADS_LIB_RELEASE}")
endif()

if(WIN32 AND NOT MSVC)
    message(WARNING "Qt/QtADS libraries in this project are MSVC builds. Configure with an MSVC Qt kit (for example Qt 6.8.3 msvc2022_64) for successful linking.")
endif()

add_library(qtadvanceddocking STATIC IMPORTED GLOBAL)
set_target_properties(qtadvanceddocking PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${QTADS_DIR}/include"
    IMPORTED_LOCATION "${QTADS_LIB_RELEASE}"
    IMPORTED_LOCATION_DEBUG "${QTADS_LIB_DEBUG}"
    IMPORTED_LOCATION_RELEASE "${QTADS_LIB_RELEASE}"
    MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
    MAP_IMPORTED_CONFIG_MINSIZEREL Release
)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
)

qt_add_executable(ui_design_QtADS_Template
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ui_design_Template APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

target_include_directories(ui_design_QtADS_Template PRIVATE "${QTADS_DIR}/include")
target_compile_definitions(ui_design_QtADS_Template PRIVATE ADS_STATIC)

target_link_libraries(ui_design_QtADS_Template PRIVATE
    Qt6::Widgets
    qtadvanceddocking
    IdeShell::IdeShell
)

set_target_properties(ui_design_QtADS_Template PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ui_design_QtADS_Template
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ui_design_QtADS_Template
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(ui_design_QtADS_Template)
