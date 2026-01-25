#pragma once

#include <QMainWindow>
#include <QMap>
#include <QStackedWidget>

namespace ads {
class CDockManager;
class CDockWidget;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    ads::CDockManager* m_dockManager = nullptr;
    QMap<QString, ads::CDockWidget*> m_docks;
    QMenu* m_viewMenu = nullptr;
    QMap<QString, QAction*> m_dockActions;
    QMap<QString, QByteArray> m_workspaces;

    QStackedWidget* m_centralStack = nullptr;
    QWidget* m_welcomePage = nullptr;

    void registerDock(ads::CDockWidget* dock);
    void DockCreation(void);

    void createViewMenu();
    void saveLayout();
    bool hasAnyDockVisible() const;
    void restoreLayout();

    void createWelcomePage();
    void showWelcomePage();
    void showDockLayout();
    void createHelpMenu();
};

