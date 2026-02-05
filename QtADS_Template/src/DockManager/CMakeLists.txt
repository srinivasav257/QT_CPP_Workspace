# DockManager - Reusable Qt Advanced Docking System Module
# This module provides a complete dock management framework that can be
# copied to any Qt project for instant IDE-style docking capabilities.

set(DOCKMANAGER_HEADERS
    include/DockFramework.h
    include/PanelDefinition.h
    include/PanelRegistry.h
    include/DockMainWindow.h
    include/WorkspaceManager.h
    include/DockToolBar.h
    include/CustomDockComponentsFactory.h
)

set(DOCKMANAGER_SOURCES
    src/PanelRegistry.cpp
    src/DockMainWindow.cpp
    src/WorkspaceManager.cpp
    src/DockToolBar.cpp
    src/CustomDockComponentsFactory.cpp
)

# Create static library
add_library(DockManager STATIC ${DOCKMANAGER_HEADERS} ${DOCKMANAGER_SOURCES})

# Public include directory - available to consumers
target_include_directories(DockManager PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link dependencies - these are PUBLIC so consumers get them automatically
target_link_libraries(DockManager PUBLIC
    ads::qtadvanceddocking
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# C++ standard
target_compile_features(DockManager PUBLIC cxx_std_17)

# Alias target for modern CMake usage
add_library(DockManager::DockManager ALIAS DockManager)

# Set folder for IDE organization
set_target_properties(DockManager PROPERTIES FOLDER "Libraries")
