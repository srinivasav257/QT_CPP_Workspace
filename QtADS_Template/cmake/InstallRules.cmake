include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

function(setup_install_rules project_name target_name)
    
    # 1. Standardize Install Paths
    #    - Binaries -> bin/
    #    - Libraries -> lib/
    #    - Headers   -> include/
    
    install(TARGETS ${target_name}
        EXPORT ${project_name}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        BUNDLE DESTINATION .
    )

    # 2. Generate <Project>ConfigVersion.cmake
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${project_name}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    # 3. Create a basic <Project>Config.cmake
    #    (For a simple template, we usually just include the targets file)
    set(CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/${project_name}Config.cmake")
    file(WRITE "${CONFIG_FILE}" 
         "include(\"\${CMAKE_CURRENT_LIST_DIR}/${project_name}Targets.cmake\")\n")

    # 4. Install Config files
    set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${project_name}")
    
    install(FILES
        "${CONFIG_FILE}"
        "${CMAKE_CURRENT_BINARY_DIR}/${project_name}ConfigVersion.cmake"
        DESTINATION "${CONFIG_INSTALL_DIR}"
    )

    # 5. Export Targets
    #    This allows other CMake projects to do: find_package(QtTemplateProject)
    install(EXPORT ${project_name}Targets
        FILE ${project_name}Targets.cmake
        NAMESPACE ${project_name}::
        DESTINATION "${CONFIG_INSTALL_DIR}"
    )
    
    message(STATUS "Install System: Configured for ${project_name} (Target: ${target_name})")

endfunction()
