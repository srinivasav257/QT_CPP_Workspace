#pragma once

/**
 * @file DockFramework.h
 * @brief Master include header for the DockManager module.
 *
 * Include this single header to get all DockManager functionality:
 * @code
 * #include <DockManager/DockFramework.h>
 *
 * int main(int argc, char *argv[]) {
 *     QApplication app(argc, argv);
 *
 *     // Register your panels
 *     DockManager::PanelRegistry::instance().registerPanel({...});
 *
 *     // Create and show main window
 *     DockManager::DockMainWindow window;
 *     window.show();
 *
 *     return app.exec();
 * }
 * @endcode
 */

#include "PanelDefinition.h"
#include "PanelRegistry.h"
#include "DockMainWindow.h"
#include "WorkspaceManager.h"
#include "DockToolBar.h"
#include "CustomDockComponentsFactory.h"
