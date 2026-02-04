# Define source files explicitly for reliable incremental builds
set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    PanelRegistry.cpp
    PanelRegistry.h
    panels/SamplePanels.cpp
    panels/SamplePanels.h
    "${CMAKE_SOURCE_DIR}/resources/resources.qrc"
)

# Create the executable target
add_executable(QtTemplateApp ${PROJECT_SOURCES})

# --- Qt Advanced Docking System (Static Library) ---
set(QTADS_DIR "${CMAKE_SOURCE_DIR}/third_party/qtads")

target_include_directories(QtTemplateApp PRIVATE
    "${QTADS_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/panels"
)

# Since QtADS is a static lib, define ADS_STATIC so ADS_EXPORT resolves to nothing
target_compile_definitions(QtTemplateApp PRIVATE ADS_STATIC)

# Link the correct QtADS static lib for Debug/Release
target_link_libraries(QtTemplateApp PRIVATE
    "$<$<CONFIG:Debug>:${QTADS_DIR}/lib/debug/qtadvanceddocking-qt6d_static.lib>"
    "$<$<CONFIG:Release>:${QTADS_DIR}/lib/release/qtadvanceddocking-qt6_static.lib>"
    "$<$<CONFIG:RelWithDebInfo>:${QTADS_DIR}/lib/release/qtadvanceddocking-qt6_static.lib>"
    "$<$<CONFIG:MinSizeRel>:${QTADS_DIR}/lib/release/qtadvanceddocking-qt6_static.lib>"
)

# Link libraries
target_link_libraries(QtTemplateApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Apply settings via Interface Targets
set_project_warnings(QtTemplateApp)
enable_sanitizers(QtTemplateApp)
target_include_version(QtTemplateApp)

# Precompiled Headers (Speed up compilation)
enable_pch(QtTemplateApp HEADERS
    <QApplication>
    <QMainWindow>
    <QLabel>
    <QString>
    <vector>
    <memory>
)

# Platform specific settings
if(WIN32)
    set_target_properties(QtTemplateApp PROPERTIES WIN32_EXECUTABLE ON)
endif()

# Configure Installation & Exports (must come BEFORE deployment for correct install order)
# This generates QtTemplateProjectConfig.cmake so others can find_package() it
setup_install_rules(QtTemplateProject QtTemplateApp)

# Enable deployment targets (make QtTemplateApp_deploy)
# Note: Must be after setup_install_rules() so windeployqt runs after exe is installed
add_deployment_support(QtTemplateApp)

# Generate typed Environment Header (env_config.h)
generate_env_header(QtTemplateApp)
