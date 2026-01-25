#include "mainwindow.h"
#include <QDebug>
#include <DockManager.h>
#include <QLabel>
#include <QTextEdit>
#include <QMenuBar>
#include <QSettings>
#include <QCloseEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    /* --- 1. SETUP Window tittle and Icon ---*/
    QString version = "1.0.0";
    setWindowTitle("SPYDER AutoTraceTool " + version);

    QIcon winIcon(":/ICONS/Resource/icons/app_icon.svg");
    winIcon.addFile(":/ICONS/Resource/icons/app_icon_square.svg");
    setWindowIcon(winIcon);

    DockCreation();

    createViewMenu();
    createHelpMenu();

    QSettings settings("SPYDER", "AutoTraceTool");

    if (settings.contains("layout/main"))
    {
        restoreLayout();
        showDockLayout();
    }
    else
    {
        showWelcomePage();
    }
}

MainWindow::~MainWindow() {
    qDebug() << " Program ended";
}

void MainWindow:: DockCreation()
{
    m_centralStack = new QStackedWidget(this);
    setCentralWidget(m_centralStack);

    m_dockManager = new ads::CDockManager(m_centralStack);
    // setCentralWidget(m_dockManager);
    m_centralStack->addWidget(m_dockManager);

    // Create welcome page
    createWelcomePage();

    /* QStackedWidget
     ├─ index 0 → Welcome page
     └─ index 1 → ADS DockManager */
    m_centralStack->insertWidget(0, m_welcomePage);

    // ---------- Left Dock ----------
    auto* projectLabel = new QLabel("Project Explorer");
    projectLabel->setAlignment(Qt::AlignCenter);

    auto* projectDock = new ads::CDockWidget("Project");
    projectDock->setObjectName("dock.project");
    projectDock->setWidget(projectLabel);
    m_dockManager->addDockWidget(ads::DockWidgetArea::LeftDockWidgetArea, projectDock);
    projectDock->closeDockWidget();
    registerDock(projectDock);

    // ---------- Center Dock ----------
    auto* centerText = new QTextEdit();
    centerText->setPlainText("CAN Messages View");
    centerText->setAlignment(Qt::AlignCenter);

    auto* centerDock = new ads::CDockWidget("CAN Messages");
    centerDock->setObjectName("dock.can_messages");
    centerDock->setWidget(centerText);
    m_dockManager->addDockWidget(ads::DockWidgetArea::CenterDockWidgetArea, centerDock);
    centerDock->closeDockWidget();
    registerDock(centerDock);

    // ---------- Right Dock ----------
    auto* propertiesLabel = new QLabel("Properties Panel");
    propertiesLabel->setAlignment(Qt::AlignCenter);

    auto* propertiesDock = new ads::CDockWidget("Properties");
    propertiesDock->setObjectName("dock.properties");
    propertiesDock->setWidget(propertiesLabel);
    m_dockManager->addDockWidget(ads::DockWidgetArea::RightDockWidgetArea, propertiesDock);
    propertiesDock->closeDockWidget();
    registerDock(propertiesDock);

    // ---------- Bottom Dock ----------
    auto* logText = new QTextEdit();
    logText->setPlainText("Log Output");
    logText->setAlignment(Qt::AlignCenter);


    auto* logDock = new ads::CDockWidget("Log");
    logDock->setObjectName("dock.log");
    logDock->setWidget(logText);
    m_dockManager->addDockWidget(ads::DockWidgetArea::BottomDockWidgetArea, logDock);
    logDock->closeDockWidget();
    registerDock(logDock);
}

void MainWindow::registerDock(ads::CDockWidget* dock)
{
    if(dock!=nullptr)
    {
        qDebug() << "The Dock ID: " << dock->objectName();
        m_docks.insert(dock->objectName(), dock);
    }
    else
    {
        qDebug() << "Invalid Dock ";
    }
}

void MainWindow::createViewMenu()
{
    m_viewMenu = menuBar()->addMenu(tr("&View"));

    for (ads::CDockWidget* dock : m_docks)
    {
        qDebug() << "Menubar Docke ID: " << dock;
        QAction* action = dock->toggleViewAction();
        action->setText(dock->windowTitle());
        m_viewMenu->addAction(action);
        connect(action, &QAction::triggered,this, &MainWindow::showDockLayout);
    }
}

void MainWindow::saveLayout()
{
    if (!hasAnyDockVisible())
        return;
    QSettings settings("SPYDER", "AutoTraceTool");
    settings.setValue("layout/main", m_dockManager->saveState());
}

bool MainWindow::hasAnyDockVisible() const
{
    for (ads::CDockWidget* dock : m_docks)
    {
        if (dock->isVisible())
            return true;
    }
    return false;
}

void MainWindow::restoreLayout()
{
    QSettings settings("SPYDER", "AutoTraceTool");

    if (settings.contains("layout/main"))
    {
        QByteArray layout = settings.value("layout/main").toByteArray();
        m_dockManager->restoreState(layout);
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveLayout();
    QMainWindow::closeEvent(event);
}

void MainWindow::createHelpMenu()
{
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction* aboutAction = helpMenu->addAction(tr("About SPYDER"));
    connect(aboutAction, &QAction::triggered, this, [this](){
        // Simple placeholder for an About dialog
        qDebug() << "About clicked";
    });
}

void MainWindow::createWelcomePage()
{
    m_welcomePage = new QWidget(this);

    auto* layout = new QVBoxLayout(m_welcomePage);
    layout->setAlignment(Qt::AlignCenter);

    auto* title = new QLabel("Welcome to SPYDER");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");

    auto* subtitle = new QLabel("Open a tool to get started");
    subtitle->setStyleSheet("color: gray;");

    auto* hint = new QLabel("Use View menu to open panels");

    layout->addWidget(title);
    layout->addSpacing(10);
    layout->addWidget(subtitle);
    layout->addSpacing(20);
    layout->addWidget(hint);
}

void MainWindow::showWelcomePage()
{
    m_centralStack->setCurrentWidget(m_welcomePage);
}

void MainWindow::showDockLayout()
{
    m_centralStack->setCurrentWidget(m_dockManager);
}
