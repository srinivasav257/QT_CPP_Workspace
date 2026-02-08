#include "mainwindow.h"

#include "DockManager.h"
#include "DockWidget.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : ide_shell::IdeShellWindow(parent)
{
    setupDockingArea();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupDockingArea()
{
    ads::CDockManager::setConfigFlags(ads::CDockManager::DefaultOpaqueConfig);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasTabsMenuButton, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::EqualSplitOnInsertion, true);
    ads::CDockManager::setAutoHideConfigFlags(ads::CDockManager::DefaultAutoHideConfig);

    m_dockManager = new ads::CDockManager(workspaceHost());
    setWorkspaceWidget(m_dockManager);

    auto *welcomeDock = new ads::CDockWidget(m_dockManager, "Welcome");
    welcomeDock->setWidget(createWelcomePanel(), ads::CDockWidget::ForceNoScrollArea);
    welcomeDock->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    welcomeDock->setFeature(ads::CDockWidget::DockWidgetMovable, false);
    welcomeDock->setFeature(ads::CDockWidget::DockWidgetFloatable, false);
    m_dockManager->addDockWidget(ads::CenterDockWidgetArea, welcomeDock);

    auto *explorerList = new QListWidget();
    explorerList->setObjectName("panelList");
    explorerList->addItems({"project", "src", "include", "CMakeLists.txt", "README.md"});

    auto *explorerDock = new ads::CDockWidget(m_dockManager, "Explorer");
    explorerDock->setWidget(explorerList, ads::CDockWidget::ForceNoScrollArea);
    auto *leftArea = m_dockManager->addDockWidget(ads::LeftDockWidgetArea, explorerDock);

    auto *searchPanel = new QWidget();
    searchPanel->setObjectName("panelWidget");
    auto *searchLayout = new QVBoxLayout(searchPanel);
    searchLayout->setContentsMargins(8, 8, 8, 8);
    searchLayout->setSpacing(8);

    auto *searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Search project files");
    searchLayout->addWidget(searchInput);

    auto *searchResults = new QListWidget();
    searchResults->setObjectName("panelList");
    searchResults->addItems({"mainwindow.cpp", "mainwindow.h", "main.cpp", "CMakeLists.txt"});
    searchLayout->addWidget(searchResults);

    auto *searchDock = new ads::CDockWidget(m_dockManager, "Search");
    searchDock->setWidget(searchPanel, ads::CDockWidget::ForceNoScrollArea);
    m_dockManager->addDockWidgetTabToArea(searchDock, leftArea);

    auto *outlineList = new QListWidget();
    outlineList->setObjectName("panelList");
    outlineList->addItems({"MainWindow", "setupDockingArea", "IdeShellWindow", "createWelcomePanel"});

    auto *outlineDock = new ads::CDockWidget(m_dockManager, "Outline");
    outlineDock->setWidget(outlineList, ads::CDockWidget::ForceNoScrollArea);
    m_dockManager->addDockWidget(ads::RightDockWidgetArea, outlineDock);

    auto *problemsView = new QTreeWidget();
    problemsView->setObjectName("problemsTree");
    problemsView->setColumnCount(3);
    problemsView->setRootIsDecorated(false);
    problemsView->setAlternatingRowColors(true);
    problemsView->setHeaderLabels({"File", "Line", "Message"});
    problemsView->header()->setStretchLastSection(true);

    auto *issueA = new QTreeWidgetItem({"mainwindow.cpp", "29", "Dummy warning: style token mismatch"});
    auto *issueB = new QTreeWidgetItem({"CMakeLists.txt", "58", "Dummy note: release profile"});
    problemsView->addTopLevelItem(issueA);
    problemsView->addTopLevelItem(issueB);

    auto *problemsDock = new ads::CDockWidget(m_dockManager, "Problems");
    problemsDock->setWidget(problemsView, ads::CDockWidget::ForceNoScrollArea);
    auto *bottomArea = m_dockManager->addDockWidget(ads::BottomDockWidgetArea, problemsDock);

    auto *terminalView = new QPlainTextEdit();
    terminalView->setObjectName("terminalView");
    terminalView->setReadOnly(true);
    terminalView->setPlainText("PS C:\\workspace> cmake --build .\nBuild completed (dummy output)\n");

    auto *terminalDock = new ads::CDockWidget(m_dockManager, "Terminal");
    terminalDock->setWidget(terminalView, ads::CDockWidget::ForceNoScrollArea);
    m_dockManager->addDockWidgetTabToArea(terminalDock, bottomArea);
}
