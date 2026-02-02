# Auto-detect UI and Resource files
file(GLOB_RECURSE PROJECT_UIS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")
# Note: Resources are often shared, so we search in the project root's resources folder
file(GLOB_RECURSE PROJECT_RESOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/resources/*.qrc")

# Define source files
set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    ${PROJECT_UIS}
    ${PROJECT_RESOURCES}
)

# Create the executable target
add_executable(QtTemplateApp ${PROJECT_SOURCES})

# Link libraries
# Clean modern target-based linking
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
