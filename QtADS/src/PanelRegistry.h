#pragma once

#include <QString>
#include <QIcon>
#include <QMap>
#include <QList>
#include <functional>
#include "ads_globals.h"

class QWidget;

// Describes a panel type that can be instantiated by the dock system.
// Each panel has a unique ID, display name, category, default dock area,
// and a factory function that creates its content widget.
struct PanelDefinition
{
    QString id;
    QString title;
    QString category;
    ads::DockWidgetArea defaultArea = ads::CenterDockWidgetArea;
    std::function<QWidget*(QWidget*)> factory;
};

// Central registry for all panel types in the application.
//
// To add a new panel, simply call registerPanel() with a PanelDefinition.
// The MainWindow uses this registry to create dock widgets, build menus,
// and manage perspectives.
//
// Usage:
//   PanelRegistry::instance().registerPanel({
//       "my_panel", "My Panel", "Tools",
//       ads::BottomDockWidgetArea,
//       [](QWidget* parent) { return new MyWidget(parent); }
//   });
class PanelRegistry
{
public:
    static PanelRegistry& instance();

    bool registerPanel(const PanelDefinition& def);

    const PanelDefinition* panel(const QString& id) const;
    const QList<PanelDefinition>& panels() const;
    QStringList categories() const;
    QList<PanelDefinition> panelsInCategory(const QString& category) const;

private:
    PanelRegistry() = default;
    QList<PanelDefinition> m_panelList;       // preserves registration order
    QMap<QString, int>     m_idToIndex;        // ID -> index in m_panelList
};
