#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QScopedPointer>

namespace ads { class CDockManager; }

namespace DockManager {

/**
 * @brief Manages workspace state, perspectives, and layout locking.
 *
 * The WorkspaceManager handles:
 * - Saving and restoring dock layout state
 * - Managing named perspectives (layout snapshots)
 * - Workspace locking to prevent accidental layout changes
 *
 * It automatically persists state to QSettings using the application's
 * organization and name settings.
 */
class WorkspaceManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct a WorkspaceManager
     * @param dockManager The CDockManager instance to manage
     * @param parent Parent QObject
     */
    explicit WorkspaceManager(ads::CDockManager* dockManager, QObject* parent = nullptr);
    ~WorkspaceManager() override;

    // --- Perspective Management ---

    /**
     * @brief Save current layout as a named perspective
     * @param name The perspective name
     */
    void savePerspective(const QString& name);

    /**
     * @brief Load a previously saved perspective
     * @param name The perspective name
     * @return true if loaded successfully
     */
    bool loadPerspective(const QString& name);

    /**
     * @brief Remove a saved perspective
     * @param name The perspective name
     */
    void removePerspective(const QString& name);

    /**
     * @brief Get list of all saved perspective names
     */
    QStringList perspectiveNames() const;

    /**
     * @brief Get the name of the currently active perspective
     */
    QString currentPerspective() const;

    // --- State Persistence ---

    /**
     * @brief Save current dock state to QSettings
     */
    void saveState();

    /**
     * @brief Restore dock state from QSettings
     * @return true if state was restored successfully
     */
    bool restoreState();

    /**
     * @brief Save all perspectives to QSettings
     */
    void savePerspectives();

    /**
     * @brief Load all perspectives from QSettings
     */
    void loadPerspectives();

    /**
     * @brief Save window geometry to QSettings
     * @param geometry The geometry data from QMainWindow::saveGeometry()
     */
    void saveGeometry(const QByteArray& geometry);

    /**
     * @brief Get saved window geometry from QSettings
     * @return The geometry data, or empty QByteArray if not saved
     */
    QByteArray savedGeometry() const;

    // --- Workspace Locking ---

    /**
     * @brief Check if workspace is locked
     */
    bool isLocked() const;

    /**
     * @brief Lock or unlock the workspace
     * @param locked If true, prevents layout modifications
     *
     * When locked, dock widgets cannot be moved, floated, or closed.
     */
    void setLocked(bool locked);

signals:
    /**
     * @brief Emitted when a perspective is loaded
     * @param name The perspective name
     */
    void perspectiveChanged(const QString& name);

    /**
     * @brief Emitted when a perspective is saved
     * @param name The perspective name
     */
    void perspectiveSaved(const QString& name);

    /**
     * @brief Emitted when workspace lock state changes
     * @param locked The new lock state
     */
    void lockedChanged(bool locked);

private:
    struct Private;
    QScopedPointer<Private> d;
};

} // namespace DockManager
