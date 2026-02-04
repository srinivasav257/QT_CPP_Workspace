function(add_deployment_support target_name)
    
    # --- Linux / Unix RPATH Settings ---
    if(UNIX AND NOT APPLE)
        # Ensure the executable finds libraries in relative paths (e.g. ./lib or ../lib)
        set_target_properties(${target_name} PROPERTIES
            SKIP_BUILD_RPATH FALSE
            BUILD_WITH_INSTALL_RPATH FALSE
            INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib"
            INSTALL_RPATH_USE_LINK_PATH TRUE
        )
        message(STATUS "Deployment: Configured RPATH for Linux target '${target_name}'")
    endif()

    # --- Windows (windeployqt) ---
    if(WIN32)
        # Locate windeployqt
        get_target_property(QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

        if(WINDEPLOYQT_EXECUTABLE)
            # 1. Custom Target (Run manually) - Deploys to BUILD directory
            add_custom_target(${target_name}_deploy
                COMMAND ${CMAKE_COMMAND} -E echo "Running windeployqt on $<TARGET_FILE:${target_name}>..."
                COMMAND ${WINDEPLOYQT_EXECUTABLE}
                        --dir $<TARGET_FILE_DIR:${target_name}>
                        --no-translations
                        --no-compiler-runtime
                        --no-network
                        --no-system-d3d-compiler
                        --no-opengl-sw
                        $<TARGET_FILE:${target_name}>
                COMMENT "Deploying Qt dependencies for Windows..."
                VERBATIM
            )
            message(STATUS "Deployment: Added target '${target_name}_deploy' (windeployqt)")

            # 2. Install Step (Run automatically via CPack/Install) - Deploys to INSTALL directory
            # Resolve target output name at configure time to avoid generator expression
            # issues inside install(CODE) on some generators
            get_target_property(_output_name ${target_name} OUTPUT_NAME)
            if(NOT _output_name)
                set(_output_name "${target_name}")
            endif()
            set(_exe_filename "${_output_name}${CMAKE_EXECUTABLE_SUFFIX}")

            install(CODE "
                message(STATUS \"Deploying Qt dependencies to install prefix...\")
                execute_process(
                    COMMAND \"${WINDEPLOYQT_EXECUTABLE}\"
                            --dir \"\${CMAKE_INSTALL_PREFIX}/bin\"
                            --no-translations
                            --no-compiler-runtime
                            --no-network
                            --no-system-d3d-compiler
                            --no-opengl-sw
                            --libdir \"\${CMAKE_INSTALL_PREFIX}/bin\"
                            --plugindir \"\${CMAKE_INSTALL_PREFIX}/bin/plugins\"
                            \"\${CMAKE_INSTALL_PREFIX}/bin/${_exe_filename}\"
                    RESULT_VARIABLE deploy_result
                )
                if(NOT deploy_result EQUAL 0)
                    message(FATAL_ERROR \"windeployqt failed with exit code: \${deploy_result}\")
                endif()
            ")

        else()
            message(WARNING "windeployqt not found. Deployment target skipped.")
        endif()
    endif()

    # --- macOS (macdeployqt) ---
    if(APPLE)
        # Ensure it is a bundle
        set_target_properties(${target_name} PROPERTIES MACOSX_BUNDLE ON)

        # Locate macdeployqt
        get_target_property(QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)
        find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${QT_BIN_DIR}")

        if(MACDEPLOYQT_EXECUTABLE)
            add_custom_target(${target_name}_deploy
                COMMAND ${MACDEPLOYQT_EXECUTABLE} $<TARGET_BUNDLE_DIR:${target_name}> -dmg
                COMMENT "Running macdeployqt (creating DMG)..."
                VERBATIM
            )
             message(STATUS "Deployment: Added target '${target_name}_deploy' (macdeployqt)")
        endif()
    endif()

endfunction()
