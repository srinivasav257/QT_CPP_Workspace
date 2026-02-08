set(IDE_SHELL_HEADERS
    include/IdeShell/IdeShellWindow.h
)

set(IDE_SHELL_SOURCES
    src/IdeShellWindow.cpp
)

add_library(IdeShell STATIC
    ${IDE_SHELL_HEADERS}
    ${IDE_SHELL_SOURCES}
)

add_library(IdeShell::IdeShell ALIAS IdeShell)

target_include_directories(IdeShell PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(IdeShell PUBLIC Qt6::Widgets)
target_compile_features(IdeShell PUBLIC cxx_std_17)

set_target_properties(IdeShell PROPERTIES
    FOLDER "Libraries"
)
