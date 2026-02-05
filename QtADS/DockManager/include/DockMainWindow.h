#pragma once

#include <QMainWindow>
#include <QMap>
#include <QScopedPointer>

namespace ads {
    class CDockManager;
    class CDockWidget;
    class CDockAreaWidget;
}

class QMenu;
class QToolBar;

namespace DockManager {

class WorkspaceManager;
class DockToolBar;

/**
 * @brief Base main window class with integrated dock management.
 *
 * DockMainWindow provides a ready-to-use main window with:
 * - Automatic panel creation from PanelRegistry
 * - Standard menus (File, View, Perspectives, Help)
 * - Optional toolbar with workspace controls
 * - State persistence (save/restore layout on close/open)
 * - Perspective management
 *
 * Basic usage:
 * @code
 * // Register panels before creating window
 * registerMyPanels();
 *
 * // Create and show window
 * DockManager::DockMainWindow window;
 * window.setWindowTitle("My Application");
 * window.show();
 * @endcode
 *
 * Advanced usage (customize via inheritance):
 * @code
 * class MyMainWindow : public DockManager::DockMainWindow {
 * protected:
 *     void configureFlags() override {
 *         DockMainWindow::configureFlags();
 *         // Add custom configuration
 *     }
 *
 *     void createMenus() override {
 *         DockMainWindow::createMenus();
 *         // Add custom menus
 *     }
 * };
 * @endcode
 */
class DockMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Construct a DockMainWindow
     * @param parent Parent widget
     */
    explicit DockMainWindow(QWidget* parent = nullptr);
    ~DockMainWindow() override;

    // --- Accessors ---

    /**
     * @brief Get the underlying CDockManager
     */
    ads::CDockManager* dockManager() const;

    /**
     * @brief Get the workspace manager
     */
    WorkspaceManager* workspaceManager() const;

    /**
     * @brief Get the dock toolbar (may be nullptr if not created)
     */
    DockToolBar* dockToolBar() const;

    /**
     * @brief Get a dock widget by its panel ID
     * @param panelId The panel ID from PanelRegistry
     * @return The dock widget, or nullptr if not found
     */
    ads::CDockWidget* dockWidget(const QString& panelId) const;

    /**
     * @brief Get all created dock widgets
     */
    QMap<QString, ads::CDockWidget*> dockWidgets() const;

protected:
    // --- Virtual methods for customization ---

    /**
     * @brief Configure dock manager flags.
     *
     * Override to customize dock manager behavior.
     * Called before the dock manager is created.
     *
     * Default implementation enables:
     * - FocusHighlighting
     * - DockAreaHasTabsMenuButton
     * - DockAreaHasUndockButton
     * - AlwaysShowTabs
     * - EqualSplitOnInsertion
     * - Auto-hide (sidebar pinning)
     */
    virtual void configureFlags();

    /**
     * @brief Create dock panels from the PanelRegistry.
     *
     * Override to customize panel creation or filter panels.
     */
    virtual void createPanels();

    /**
     * @brief Set up the default layout.
     *
     * Override to customize the initial panel arrangement.
     * Default implementation uses a two-pass algorithm:
     * 1. First panel per area establishes the dock area
     * 2. Remaining panels are tabbed into their area
     */
    virtual void setupDefaultLayout();

    /**
     * @brief Create the menu bar.
     *
     * Override to add custom menus. Call base implementation first
     * to get standard File, View, Perspectives, and Help menus.
     */
    virtual void createMenus();

    /**
     * @brief Create the dock toolbar.
     *
     * Override to customize the toolbar or prevent its creation
     * by not calling base implementation.
     */
    virtual void createToolBar();

    /**
     * @brief Called after all initialization is complete.
     *
     * Override to perform additional setup after panels and
     * menus have been created.
     */
    virtual void initializeComplete();

    /**
     * @brief Close event handler - saves state automatically
     */
    void closeEvent(QCloseEvent* event) override;

    // --- Protected accessors for derived classes ---

    /**
     * @brief Get the central dock area
     */
    ads::CDockAreaWidget* centralArea() const;

    /**
     * @brief Get the perspectives menu for customization
     */
    QMenu* perspectiveMenu() const;

protected slots:
    /**
     * @brief Rebuild the perspectives menu (after perspective changes)
     */
    void rebuildPerspectiveMenu();

private:
    struct Private;
    QScopedPointer<Private> d;
};

} // namespace DockManager
