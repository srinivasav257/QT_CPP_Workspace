option(ENABLE_QML_SUPPORT "Enable QML and Quick modules" OFF)

if(ENABLE_QML_SUPPORT)
    find_package(Qt6 COMPONENTS Qml Quick QuickControls2 QUIET)
    
    if(Qt6Qml_FOUND)
        message(STATUS "Qt6 Qml/Quick found. QML support enabled.")
        
        # Helper to find the 'qml' tool
        get_target_property(QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)
        find_program(QML_EXECUTABLE NAMES qml scene qmlscene HINTS ${QT_BIN_DIR})

        # Function to add QML Live targets
        function(enable_qml_support target_name qml_src_dir)
            
            # 1. Define preprocessor for C++ to know where source is (for filesystem loading)
            target_compile_definitions(${target_name} PRIVATE 
                QML_SRC_DIR="${qml_src_dir}"
                QML_DEBUG_ENABLED
            )

            # 2. Add 'qml_live' target (runs qml tool)
            if(QML_EXECUTABLE)
                add_custom_target(${target_name}_qml_live
                    COMMAND ${QML_EXECUTABLE} -I "${qml_src_dir}" "${qml_src_dir}/main.qml"
                    WORKING_DIRECTORY "${qml_src_dir}"
                    COMMENT "Running QML Scene on ${qml_src_dir}/main.qml"
                    VERBATIM
                )
                message(STATUS "Added target: ${target_name}_qml_live")
            endif()

            # 3. Add 'qml_watch' target (runs python watcher)
            find_package(Python3 QUIET)
            if(Python3_FOUND)
                 # Determine where to sync files. 
                 # Often, apps copy QML to binary dir in build.
                 # We assume the app is loading from ${CMAKE_CURRENT_BINARY_DIR} if deploying.
                 set(QML_DEST_DIR "${CMAKE_BINARY_DIR}/qml_staging")
                 
                 add_custom_target(${target_name}_qml_watch
                    COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/qml_watcher.py" "${qml_src_dir}" "${QML_DEST_DIR}"
                    COMMENT "Watching QML files in ${qml_src_dir} -> ${QML_DEST_DIR}"
                    VERBATIM
                 )
            endif()

        endfunction()

    else()
        message(WARNING "ENABLE_QML_SUPPORT is ON, but Qt6::Quick not found.")
    endif()
endif()
