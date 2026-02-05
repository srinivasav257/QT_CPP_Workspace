#pragma once

#include "PanelDefinition.h"
#include <QList>
#include <QMap>
#include <QStringList>

namespace DockManager {

/**
 * @brief Central registry for all panel types in the application.
 *
 * The PanelRegistry is a singleton that holds definitions for all panels
 * that can be created in the dock system. Register panels before creating
 * the DockMainWindow.
 *
 * Usage:
 * @code
 * // In your application startup (before creating MainWindow):
 * auto& reg = DockManager::PanelRegistry::instance();
 * reg.registerPanel({
 *     .id = "my_panel",
 *     .title = "My Panel",
 *     .category = "Tools",
 *     .defaultArea = ads::BottomDockWidgetArea,
 *     .factory = [](QWidget* parent) { return new MyWidget(parent); }
 * });
 * @endcode
 */
class PanelRegistry
{
public:
    /**
     * @brief Get the singleton instance
     */
    static PanelRegistry& instance();

    /**
     * @brief Register a new panel type
     * @param def The panel definition
     * @return true if registered successfully, false if ID already exists
     */
    bool registerPanel(const PanelDefinition& def);

    /**
     * @brief Register a panel using move semantics (for efficiency with lambdas)
     */
    bool registerPanel(PanelDefinition&& def);

    /**
     * @brief Look up a panel definition by ID
     * @param id The panel ID
     * @return Pointer to the definition, or nullptr if not found
     */
    const PanelDefinition* panel(const QString& id) const;

    /**
     * @brief Get all registered panels (preserves registration order)
     * @return Const reference to the panel list
     */
    const QList<PanelDefinition>& panels() const;

    /**
     * @brief Get all unique category names (sorted alphabetically)
     */
    QStringList categories() const;

    /**
     * @brief Get all panels in a specific category (preserves registration order)
     * @param category The category name
     * @return List of panel definitions in the category
     */
    QList<PanelDefinition> panelsInCategory(const QString& category) const;

    /**
     * @brief Check if a panel ID is already registered
     */
    bool contains(const QString& id) const;

    /**
     * @brief Get the number of registered panels
     */
    int count() const;

    /**
     * @brief Clear all registered panels (mainly for testing)
     */
    void clear();

private:
    PanelRegistry() = default;
    PanelRegistry(const PanelRegistry&) = delete;
    PanelRegistry& operator=(const PanelRegistry&) = delete;

    QList<PanelDefinition> m_panelList;  // Preserves registration order
    QMap<QString, int> m_idToIndex;       // ID -> index in m_panelList
};

} // namespace DockManager
