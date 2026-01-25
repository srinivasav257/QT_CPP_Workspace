#pragma once

#include <QObject>
#include <QWidget>
#include <DockManager.h>
#include <QMap>
#include "dockwidgetfactory.h"

namespace ads { class CDockWidget; }

class AppDockManager : public QObject
{
    Q_OBJECT
public:
    explicit AppDockManager(QWidget* parent = nullptr);
    ~AppDockManager();

    ads::CDockManager* dockManager() const;

    // Dock creation using factory pattern
    void createDock(DockType type);
    void createAllDocks();

    // Dock management
    ads::CDockWidget* getDock(DockType type) const;
    void showDock(DockType type);
    void hideDock(DockType type);
    bool isDockVisible(DockType type) const;

    // Menu integration
    QList<QAction*> viewMenuActions();
    QMap<QString, QList<QAction*>> getGroupedMenuActions();

    // Layout persistence
    void saveLayout();
    void restoreLayout();
    bool hasSavedLayout() const;
    void loadDefaultLayout();

signals:
    void dockActivated(DockType type);
    void firstDockOpened();

private:
    void onDockViewToggled(DockType type, bool visible);
    void registerDock(DockType type, ads::CDockWidget* dock);

    ads::CDockManager* m_adsManager = nullptr;
    QMap<DockType, ads::CDockWidget*> m_docks;
    QMap<QString, ads::CDockWidget*> m_docksByName;
    bool m_anyDockOpened = false;
};
