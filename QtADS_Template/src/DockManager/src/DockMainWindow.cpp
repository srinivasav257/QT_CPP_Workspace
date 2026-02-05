#include "DockMainWindow.h"
#include "PanelRegistry.h"
#include "WorkspaceManager.h"
#include "DockToolBar.h"

#include "DockManager.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

namespace DockManager {

struct DockMainWindow::Private
{
    ads::CDockManager* dockManager = nullptr;
    WorkspaceManager* workspaceManager = nullptr;
    DockToolBar* dockToolBar = nullptr;

    QMap<QString, ads::CDockWidget*> dockWidgets;
    ads::CDockAreaWidget* centralArea = nullptr;
    QMenu* perspectiveMenu = nullptr;
};

DockMainWindow::DockMainWindow(QWidget* parent)
    : QMainWindow(parent)
    , d(new Private)
{
    // Default window setup
    resize(1400, 900);
    statusBar()->showMessage(tr("Ready"));

    // Initialize in order
    configureFlags();

    // Create dock manager
    d->dockManager = new ads::CDockManager(this);

    // Create workspace manager
    d->workspaceManager = new WorkspaceManager(d->dockManager, this);

    // Create panels and layout
    createPanels();
    createMenus();
    createToolBar();
    setupDefaultLayout();

    // Load saved perspectives FIRST (before saving Default)
    d->workspaceManager->loadPerspectives();

    // Save default layout as "Default" perspective (only if not already present)
    if (!d->workspaceManager->perspectiveNames().contains("Default")) {
        d->workspaceManager->savePerspective("Default");
    }

    // Try to restore previous session
    auto savedGeometry = d->workspaceManager->savedGeometry();
    if (!savedGeometry.isEmpty()) {
        restoreGeometry(savedGeometry);
    }

    d->workspaceManager->restoreState();

    // Update perspective menu
    rebuildPerspectiveMenu();

    // Signal initialization complete
    initializeComplete();
}

DockMainWindow::~DockMainWindow() = default;

// --- Accessors ---

ads::CDockManager* DockMainWindow::dockManager() const
{
    return d->dockManager;
}

WorkspaceManager* DockMainWindow::workspaceManager() const
{
    return d->workspaceManager;
}

DockToolBar* DockMainWindow::dockToolBar() const
{
    return d->dockToolBar;
}

ads::CDockWidget* DockMainWindow::dockWidget(const QString& panelId) const
{
    return d->dockWidgets.value(panelId);
}

QMap<QString, ads::CDockWidget*> DockMainWindow::dockWidgets() const
{
    return d->dockWidgets;
}

ads::CDockAreaWidget* DockMainWindow::centralArea() const
{
    return d->centralArea;
}

QMenu* DockMainWindow::perspectiveMenu() const
{
    return d->perspectiveMenu;
}

// --- Virtual Implementation Methods ---

void DockMainWindow::configureFlags()
{
    // Configure flags BEFORE creating the dock manager
    ads::CDockManager::setConfigFlags(ads::CDockManager::DefaultOpaqueConfig);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasTabsMenuButton, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::EqualSplitOnInsertion, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaDynamicTabsMenuButtonVisibility, true);

    // Enable auto-hide (sidebar pinning)
    ads::CDockManager::setAutoHideConfigFlags(ads::CDockManager::DefaultAutoHideConfig);
}

void DockMainWindow::createPanels()
{
    const auto& panels = PanelRegistry::instance().panels();

    for (const auto& def : panels) {
        // Use def.id as the object name for state save/restore
        auto* dockWidget = new ads::CDockWidget(def.id, d->dockManager);
        dockWidget->setWindowTitle(def.title);

        // Set icon if provided
        if (!def.icon.isNull()) {
            dockWidget->setIcon(def.icon);
        }

        // Create content widget using factory
        QWidget* content = def.factory(dockWidget);
        dockWidget->setWidget(content, ads::CDockWidget::ForceNoScrollArea);

        // Configure features
        dockWidget->setFeatures(def.features);
        dockWidget->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, false);
        dockWidget->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromContent);

        d->dockWidgets.insert(def.id, dockWidget);
    }
}

void DockMainWindow::setupDefaultLayout()
{
    ads::CDockAreaWidget* leftArea = nullptr;
    ads::CDockAreaWidget* rightArea = nullptr;
    ads::CDockAreaWidget* bottomArea = nullptr;
    ads::CDockAreaWidget* centerArea = nullptr;

    const auto& panels = PanelRegistry::instance().panels();

    // Pass 1: Place the first panel per area to establish the dock areas
    for (const auto& def : panels) {
        auto* dw = d->dockWidgets.value(def.id);
        if (!dw) continue;

        switch (def.defaultArea) {
        case ads::LeftDockWidgetArea:
            if (!leftArea)
                leftArea = d->dockManager->addDockWidget(ads::LeftDockWidgetArea, dw);
            break;
        case ads::RightDockWidgetArea:
            if (!rightArea)
                rightArea = d->dockManager->addDockWidget(ads::RightDockWidgetArea, dw);
            break;
        case ads::BottomDockWidgetArea:
            if (!bottomArea)
                bottomArea = d->dockManager->addDockWidget(ads::BottomDockWidgetArea, dw);
            break;
        case ads::CenterDockWidgetArea:
        default:
            if (!centerArea)
                centerArea = d->dockManager->addDockWidget(ads::CenterDockWidgetArea, dw);
            break;
        }
    }

    d->centralArea = centerArea;

    // Pass 2: Tab remaining panels into their area
    for (const auto& def : panels) {
        auto* dw = d->dockWidgets.value(def.id);
        if (!dw || dw->dockAreaWidget()) continue; // already placed

        ads::CDockAreaWidget* targetArea = nullptr;
        switch (def.defaultArea) {
        case ads::LeftDockWidgetArea:   targetArea = leftArea;   break;
        case ads::RightDockWidgetArea:  targetArea = rightArea;  break;
        case ads::BottomDockWidgetArea: targetArea = bottomArea; break;
        default:                        targetArea = centerArea; break;
        }

        if (targetArea)
            d->dockManager->addDockWidgetTabToArea(dw, targetArea);
        else
            d->dockManager->addDockWidget(def.defaultArea, dw);
    }
}

void DockMainWindow::createMenus()
{
    // --- File menu ---
    auto* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("Save Layout"), this, [this]() {
        d->workspaceManager->saveState();
        statusBar()->showMessage(tr("Layout saved"), 3000);
    });
    fileMenu->addAction(tr("Restore Layout"), this, [this]() {
        d->workspaceManager->restoreState();
        statusBar()->showMessage(tr("Layout restored"), 3000);
    });
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), QKeySequence::Quit, this, &QMainWindow::close);

    // --- View menu (auto-populated by category) ---
    auto* viewMenu = menuBar()->addMenu(tr("&View"));

    // Add toggle actions grouped by category
    for (const auto& category : PanelRegistry::instance().categories()) {
        auto* catMenu = viewMenu->addMenu(category);
        for (const auto& def : PanelRegistry::instance().panelsInCategory(category)) {
            auto* dw = d->dockWidgets.value(def.id);
            if (dw)
                catMenu->addAction(dw->toggleViewAction());
        }
    }

    viewMenu->addSeparator();
    viewMenu->addAction(tr("Show All Panels"), this, [this]() {
        for (auto* dw : d->dockWidgets)
            dw->toggleView(true);
    });
    viewMenu->addAction(tr("Hide All Panels"), this, [this]() {
        for (auto* dw : d->dockWidgets)
            dw->toggleView(false);
    });

    // --- Perspectives menu ---
    d->perspectiveMenu = menuBar()->addMenu(tr("&Perspectives"));

    // Connect to workspace manager for menu updates
    connect(d->workspaceManager, &WorkspaceManager::perspectiveSaved,
            this, &DockMainWindow::rebuildPerspectiveMenu);

    // --- Help menu ---
    auto* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("About"), this, [this]() {
        QMessageBox::about(this, tr("About"),
            tr("<h3>%1</h3>"
               "<p>Qt Advanced Docking System Template</p>"
               "<p>Panels registered: %2<br>"
               "Categories: %3</p>")
            .arg(QApplication::applicationName())
            .arg(PanelRegistry::instance().count())
            .arg(PanelRegistry::instance().categories().join(", ")));
    });
}

void DockMainWindow::createToolBar()
{
    d->dockToolBar = new DockToolBar(d->workspaceManager, this);
    addToolBar(d->dockToolBar);

    // Hide all toolbar items - layout options available in menu only
    d->dockToolBar->setSaveRestoreVisible(false);
    d->dockToolBar->setPerspectivesVisible(false);
    d->dockToolBar->setLockVisible(false);
}

void DockMainWindow::initializeComplete()
{
    // Default implementation does nothing
    // Override in derived class for custom initialization
}

void DockMainWindow::rebuildPerspectiveMenu()
{
    if (!d->perspectiveMenu)
        return;

    d->perspectiveMenu->clear();

    // Save perspective action
    d->perspectiveMenu->addAction(tr("Save Perspective..."), this, [this]() {
        // DockToolBar handles this, but provide menu access too
        if (d->dockToolBar)
            d->dockToolBar->createPerspectiveAction()->trigger();
    });

    d->perspectiveMenu->addSeparator();

    // List all perspectives
    for (const auto& name : d->workspaceManager->perspectiveNames()) {
        d->perspectiveMenu->addAction(name, this, [this, name]() {
            d->workspaceManager->loadPerspective(name);
            statusBar()->showMessage(tr("Perspective '%1' loaded").arg(name), 3000);
        });
    }
}

void DockMainWindow::closeEvent(QCloseEvent* event)
{
    // Save state before closing
    d->workspaceManager->saveGeometry(saveGeometry());
    d->workspaceManager->saveState();
    d->workspaceManager->savePerspectives();

    QMainWindow::closeEvent(event);
}

} // namespace DockManager
