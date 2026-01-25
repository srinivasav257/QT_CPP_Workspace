#pragma once

#include <QObject>
#include <DockManager.h>
#include <QMap>

namespace ads { class CDockWidget; }

class AppDockManager : public QObject
{
    Q_OBJECT
public:
    // Fixed: Accept QWidget* instead of QObject*
    explicit AppDockManager(QWidget *parent = nullptr);
    ~AppDockManager();

    ads::CDockManager* dockManager() const;

    void createProjectDock();
    void createCanMessagesDock();
    void createPropertiesPanelDock();
    void createLogDock();
    
    QList<QAction*> viewMenuActions();

    // const QMap<QString, ads::CDockWidget*>& docks() const { return m_docks; }

    void saveLayout();
    void restoreLayout();
    bool hasSavedLayout() const;

signals:
    void dockActivated();

private:
    ads::CDockManager* m_adsManager = nullptr;
    QMap<QString, ads::CDockWidget*> m_docks;

};
