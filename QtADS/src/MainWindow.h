#pragma once

#include <QMainWindow>
#include <QMap>

class QMenu;
namespace ads { class CDockManager; class CDockWidget; class CDockAreaWidget; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void setupDockManager();
    void createPanels();
    void setupDefaultLayout();
    void createMenus();
    void rebuildPerspectiveMenu();

    void savePerspective();
    void saveDockState();
    bool restoreDockState();

    ads::CDockManager* m_dockManager = nullptr;
    QMap<QString, ads::CDockWidget*> m_dockWidgets;
    QMenu* m_perspectiveMenu = nullptr;

    // Track dock areas for default layout arrangement
    ads::CDockAreaWidget* m_centralArea = nullptr;
};
