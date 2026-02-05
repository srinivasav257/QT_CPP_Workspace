option(BUILD_DOCS "Build documentation using Doxygen" ON)
option(ENABLE_DOXYGEN_GRAPHVIZ "Enable Graphviz diagrams in documentation" OFF)

function(add_documentation_support)
    if(NOT BUILD_DOCS)
        message(STATUS "Documentation build disabled.")
        return()
    endif()

    find_package(Doxygen QUIET)
    if(NOT Doxygen_FOUND)
        message(WARNING "Doxygen not found. Documentation targets will not be available.")
        return()
    endif()

    find_program(PYTHON_EXECUTABLE NAMES python3 python python.exe)

    # Graphviz setup
    set(DOXYGEN_HAVE_DOT "NO")
    if(ENABLE_DOXYGEN_GRAPHVIZ)
        find_package(Doxygen COMPONENTS dot)
        if(DOXYGEN_DOT_FOUND)
            set(DOXYGEN_HAVE_DOT "YES")
        else()
            message(WARNING "Graphviz (dot) not found, but requested. Diagrams disabled.")
        endif()
    endif()

    # Configure Doxyfile
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/Doxyfile.in"
        "${CMAKE_BINARY_DIR}/Doxyfile"
        @ONLY
    )

    # Target: docs
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/Doxyfile"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating API documentation with Doxygen..."
        VERBATIM
    )

    # Target: docs-serve
    if(PYTHON_EXECUTABLE)
        add_custom_target(docs-serve
            COMMAND ${PYTHON_EXECUTABLE} -m http.server 8000
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/docs/html"
            COMMENT "Serving documentation at http://localhost:8000"
            DEPENDS docs
            VERBATIM
        )
    else()
        message(STATUS "Python not found. 'docs-serve' target not available.")
    endif()

endfunction()

# Initialize
add_documentation_support()
