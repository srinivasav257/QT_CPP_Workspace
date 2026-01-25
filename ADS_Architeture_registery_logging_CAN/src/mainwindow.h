#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenu>

class AppDockManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onFirstDockOpened();

private:
    // UI Components
    QMenu* m_viewMenu = nullptr;
    QStackedWidget* m_centralStack = nullptr;
    QWidget* m_welcomePage = nullptr;

    // Dock Management
    AppDockManager* m_appDockManager = nullptr;

    // Initialization methods
    void initializeDockSystem();
    void createMenus();
    void createViewMenu();
    void createHelpMenu();
    void createToolsMenu();
    void createFileMenu();
    void createWelcomePage();

    // View switching
    void showWelcomePage();
    void showDockLayout();
};
