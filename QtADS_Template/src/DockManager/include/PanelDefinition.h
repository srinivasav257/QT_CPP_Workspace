#pragma once

#include <QString>
#include <QIcon>
#include <functional>
#include <DockWidget.h>

class QWidget;

namespace DockManager {

/**
 * @brief Describes a panel type that can be instantiated by the dock system.
 *
 * Each panel has a unique ID, display name, category for menu grouping,
 * default dock area, and a factory function that creates its content widget.
 *
 * Basic usage (positional initialization):
 * @code
 * reg.registerPanel({
 *     "my_panel", "My Panel", "Tools",
 *     ads::LeftDockWidgetArea,
 *     [](QWidget* parent) { return new MyWidget(parent); }
 * });
 * @endcode
 *
 * Advanced usage (designated initializers with all options):
 * @code
 * reg.registerPanel({
 *     .id = "my_panel",
 *     .title = "My Panel",
 *     .category = "Tools",
 *     .defaultArea = ads::LeftDockWidgetArea,
 *     .factory = [](QWidget* parent) { return new MyWidget(parent); },
 *     .icon = QIcon(":/icons/panel.svg"),
 *     .features = ads::CDockWidget::DefaultDockWidgetFeatures
 * });
 * @endcode
 */
struct PanelDefinition
{
    /// Unique identifier used for state save/restore and lookups
    QString id;

    /// Display title shown in the dock widget title bar and menus
    QString title;

    /// Category for grouping in the View menu (e.g., "Explorer", "Debug", "Tools")
    QString category;

    /// Default dock area when creating the initial layout
    ads::DockWidgetArea defaultArea = ads::CenterDockWidgetArea;

    /// Factory function that creates the panel's content widget
    /// @param parent The parent widget (typically the CDockWidget)
    /// @return The content widget to display in the dock panel
    std::function<QWidget*(QWidget* parent)> factory;

    // --- Optional fields (with defaults) ---

    /// Optional icon displayed in tabs and title bar
    QIcon icon;

    /// Default feature flags for the dock widget
    ads::CDockWidget::DockWidgetFeatures features = ads::CDockWidget::DefaultDockWidgetFeatures;

    /// If true, only one instance of this panel can exist (default)
    /// If false, multiple instances can be created (e.g., multiple editor tabs)
    bool singleton = true;
};

} // namespace DockManager
