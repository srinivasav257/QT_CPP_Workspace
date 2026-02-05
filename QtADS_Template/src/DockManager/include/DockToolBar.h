#pragma once

#include <QToolBar>
#include <QScopedPointer>

class QAction;

namespace DockManager {

class WorkspaceManager;

/**
 * @brief Pre-configured toolbar with common dock management actions.
 *
 * The DockToolBar provides standard actions for:
 * - Save/Restore layout
 * - Perspective selection and creation
 * - Workspace locking
 *
 * Usage:
 * @code
 * auto* toolbar = new DockToolBar(workspaceManager, this);
 * addToolBar(toolbar);
 * @endcode
 */
class DockToolBar : public QToolBar
{
    Q_OBJECT

public:
    /**
     * @brief Construct a DockToolBar
     * @param workspaceManager The WorkspaceManager to control
     * @param parent Parent widget
     */
    explicit DockToolBar(WorkspaceManager* workspaceManager, QWidget* parent = nullptr);
    ~DockToolBar() override;

    // --- Visibility Control ---

    /**
     * @brief Show or hide save/restore actions
     */
    void setSaveRestoreVisible(bool visible);

    /**
     * @brief Show or hide perspective controls
     */
    void setPerspectivesVisible(bool visible);

    /**
     * @brief Show or hide lock action
     */
    void setLockVisible(bool visible);

    // --- Action Access ---

    /**
     * @brief Get the Save Layout action
     */
    QAction* saveAction() const;

    /**
     * @brief Get the Restore Layout action
     */
    QAction* restoreAction() const;

    /**
     * @brief Get the Lock/Unlock action
     */
    QAction* lockAction() const;

    /**
     * @brief Get the Create Perspective action
     */
    QAction* createPerspectiveAction() const;

private slots:
    void onCreatePerspective();
    void onLockedChanged(bool locked);

private:
    struct Private;
    QScopedPointer<Private> d;
};

} // namespace DockManager
