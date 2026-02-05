
# Packaging Configuration (CPack)
# This module configures the creation of installers (MSI, EXE, DMG, DEB, RPM)

function(setup_packaging)

    # Basic Package Metadata
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VENDOR "MyCompany")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Professional Qt CMake Template Application")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_CONTACT "support@example.com")
    
    # License File (Create a dummy one if not exists or point to COPYING)
    # set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

    # Install Directory in Program Files
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")

    # --- Generators ---
    if(WIN32)
        # Prefer NSIS (Nullsoft Scriptable Install System) if installed
        # Otherwise ZIP
        set(CPACK_GENERATOR "NSIS;ZIP")
        
        # NSIS Specifics
        set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME}")
        set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
        set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
        set(CPACK_NSIS_MODIFY_PATH ON) # Add to PATH option
        
        # Icon (Optional, uncomment if you have an .ico)
        # set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/app.ico")
        
    elseif(APPLE)
        set(CPACK_GENERATOR "DragNDrop") # .dmg
        set(CPACK_DMG_VOLUME_NAME "${CPACK_PACKAGE_NAME}")
    else()
        set(CPACK_GENERATOR "DEB;RPM;TGZ")
    endif()

    # --- Components ---
    # You can split installation into components (Core, Docs, etc.)
    # For now, we use a monolithic install.
    
    include(CPack)
    
endfunction()
