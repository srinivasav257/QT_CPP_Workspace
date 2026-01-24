#include "mainwindow.h"
#include <QDebug>
#include <DockManager.h>
#include <QLabel>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    // --- 1. SETUP Window tittle and Icon ---
    QString version = "1.0.0";
    setWindowTitle("SPYDER " + version);

    QIcon winIcon(":/ICONS/Resource/icons/app_icon.svg");
    winIcon.addFile(":/ICONS/Resource/icons/app_icon_square.svg");
    setWindowIcon(winIcon);

    DockCreation();

    Q_ASSERT(m_docks.contains("dock.project"));
    Q_ASSERT(m_docks.contains("dock.can_messages"));
    Q_ASSERT(m_docks.contains("dock.properties"));
    Q_ASSERT(m_docks.contains("dock.log"));
}

MainWindow::~MainWindow() {
    qDebug() << " Program ended";
}

void MainWindow:: DockCreation()
{
    m_dockManager = new ads::CDockManager(this);
    setCentralWidget(m_dockManager);

    // ---------- Left Dock ----------
    auto* projectLabel = new QLabel("Project Explorer");
    projectLabel->setAlignment(Qt::AlignCenter);

    auto* projectDock = new ads::CDockWidget("Project");
    projectDock->setObjectName("dock.project");
    projectDock->setWidget(projectLabel);
    m_dockManager->addDockWidget(ads::DockWidgetArea::LeftDockWidgetArea, projectDock);
    registerDock(projectDock);

    // ---------- Center Dock ----------
    auto* centerText = new QTextEdit();
    centerText->setPlainText("CAN Messages View");
    centerText->setAlignment(Qt::AlignCenter);

    auto* centerDock = new ads::CDockWidget("CAN Messages");
    centerDock->setObjectName("dock.can_messages");
    centerDock->setWidget(centerText);
    m_dockManager->addDockWidget(ads::DockWidgetArea::CenterDockWidgetArea, centerDock);
    registerDock(centerDock);

    // ---------- Right Dock ----------
    auto* propertiesLabel = new QLabel("Properties Panel");
    propertiesLabel->setAlignment(Qt::AlignCenter);

    auto* propertiesDock = new ads::CDockWidget("Properties");
    propertiesDock->setObjectName("dock.properties");
    propertiesDock->setWidget(propertiesLabel);
    m_dockManager->addDockWidget(ads::DockWidgetArea::RightDockWidgetArea, propertiesDock);
    registerDock(propertiesDock);

    // ---------- Bottom Dock ----------
    auto* logText = new QTextEdit();
    logText->setPlainText("Log Output");
    logText->setAlignment(Qt::AlignCenter);


    auto* logDock = new ads::CDockWidget("Log");
    logDock->setObjectName("dock.log");
    logDock->setWidget(logText);
    m_dockManager->addDockWidget(ads::DockWidgetArea::BottomDockWidgetArea, logDock);
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
