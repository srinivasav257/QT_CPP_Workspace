#include "mainwindow.h"
#include "appdockmanager.h"

#include <QDebug>
#include <DockManager.h>
#include <QLabel>
#include <QTextEdit>
#include <QMenuBar>
#include <QSettings>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    /* --- 1. SETUP Window title and Icon ---*/
    QString version = "1.0.0";
    setWindowTitle("SPYDER AutoTraceTool " + version);

    QIcon winIcon(":/ICONS/Resource/icons/app_icon.svg");
    winIcon.addFile(":/ICONS/Resource/icons/app_icon_square.svg");
    setWindowIcon(winIcon);

    // Fixed: Proper initialization order
    initializeDockSystem();
    createMenus();

    // Decide initial view based on saved layout
    if (m_appDockManager->hasSavedLayout())
    {
        showDockLayout();
        m_appDockManager->restoreLayout();
    }
    else
    {
        showWelcomePage();
    }
}

MainWindow::~MainWindow()
{
    qDebug() << " Program ended";
}

void MainWindow::initializeDockSystem()
{
    // 1. Create central stacked widget
    m_centralStack = new QStackedWidget(this);
    setCentralWidget(m_centralStack);

    // 2. Create welcome page FIRST
    createWelcomePage();

    // 3. Create container widget for dock manager
    QWidget *dockContainer = new QWidget(this);

    // 4. Create dock manager with proper parent
    m_appDockManager = new AppDockManager(dockContainer);

    // 5. Add widgets in correct order
    // Index 0: Welcome page
    // Index 1: Dock layout
    m_centralStack->addWidget(m_welcomePage);
    m_centralStack->addWidget(m_appDockManager->dockManager());

    // 6. Create all docks
    m_appDockManager->createProjectDock();
    m_appDockManager->createPropertiesPanelDock();
    m_appDockManager->createCanMessagesDock();
    m_appDockManager->createLogDock();

    // 7. Connect signal for dock activation
    connect(m_appDockManager, &AppDockManager::dockActivated,
            this, &MainWindow::onFirstDockOpened);
}

void MainWindow::createMenus()
{
    createViewMenu();
    createHelpMenu();
}

void MainWindow::createViewMenu()
{
    m_viewMenu = menuBar()->addMenu(tr("&View"));

    QList<QAction *> actions = m_appDockManager->viewMenuActions();
    for (QAction *action : actions)
    {
        if (action)
        {
            m_viewMenu->addAction(action);
        }
    }
}

void MainWindow::createHelpMenu()
{
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAction = helpMenu->addAction(tr("About SPYDER"));
    connect(aboutAction, &QAction::triggered, this, [this]()
            {
                qDebug() << "About clicked";
                // TODO: Implement proper About dialog
            });
}

void MainWindow::createWelcomePage()
{
    m_welcomePage = new QWidget(this);

    auto *layout = new QVBoxLayout(m_welcomePage);
    layout->setAlignment(Qt::AlignCenter);

    auto *title = new QLabel("Welcome to SPYDER AutoTraceTool");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel("Serial & CAN Testing and Monitoring Tool");
    subtitle->setStyleSheet("font-size: 14px; color: gray;");
    subtitle->setAlignment(Qt::AlignCenter);

    auto *hint = new QLabel("Use <b>View</b> menu to open panels");
    hint->setStyleSheet("font-size: 12px; margin-top: 20px;");
    hint->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(10);
    layout->addWidget(subtitle);
    layout->addSpacing(30);
    layout->addWidget(hint);
    layout->addStretch();
}

void MainWindow::showWelcomePage()
{
    if (m_centralStack && m_welcomePage)
    {
        m_centralStack->setCurrentWidget(m_welcomePage);
        qDebug() << "Showing welcome page";
    }
}

void MainWindow::showDockLayout()
{
    if (m_centralStack && m_appDockManager)
    {
        m_centralStack->setCurrentWidget(m_appDockManager->dockManager());
        qDebug() << "Showing dock layout";
    }
}

void MainWindow::onFirstDockOpened()
{
    // Only switch to dock layout if we're currently on welcome page
    if (m_centralStack && m_centralStack->currentWidget() == m_welcomePage)
    {
        showDockLayout();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_appDockManager)
    {
        m_appDockManager->saveLayout();
    }
    QMainWindow::closeEvent(event);
}