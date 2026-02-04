#include "MainWindow.h"
#include "PanelRegistry.h"

#include "DockManager.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QApplication>
#include <QStatusBar>
#include <QDebug>

static const char* kStateKey   = "MainWindow/DockState";
static const char* kGeomKey    = "MainWindow/Geometry";
static const char* kPerspGroup = "Perspectives";

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("QtADS Master Template");
    resize(1400, 900);

    statusBar()->showMessage("Ready");

    setupDockManager();
    createPanels();
    createMenus();

    // Build the default layout first (this also registers the "Default" perspective)
    setupDefaultLayout();

    // Try to restore previous session on top of the default layout
    QSettings settings;
    if (settings.contains(kStateKey)) {
        restoreDockState();
    }
}

MainWindow::~MainWindow() = default;

// ---------------------------------------------------------------------------
// Dock manager configuration
// ---------------------------------------------------------------------------
void MainWindow::setupDockManager()
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

    m_dockManager = new ads::CDockManager(this);
}

// ---------------------------------------------------------------------------
// Create dock widgets from the PanelRegistry
// ---------------------------------------------------------------------------
void MainWindow::createPanels()
{
    const auto panels = PanelRegistry::instance().panels();
    for (const auto& def : panels) {
        // Use def.id as the constructor title so the dock manager's internal
        // widget map is keyed by the same objectName used in save/restore.
        // Then set the display title via setWindowTitle().
        auto* dockWidget = new ads::CDockWidget(m_dockManager, def.id);
        dockWidget->setWindowTitle(def.title);
        dockWidget->setWidget(def.factory(dockWidget), ads::CDockWidget::ForceNoScrollArea);
        dockWidget->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, false);
        dockWidget->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromContent);

        m_dockWidgets.insert(def.id, dockWidget);
    }
}

// ---------------------------------------------------------------------------
// Default layout: place panels according to their defaultArea
// ---------------------------------------------------------------------------
void MainWindow::setupDefaultLayout()
{
    ads::CDockAreaWidget* leftArea   = nullptr;
    ads::CDockAreaWidget* rightArea  = nullptr;
    ads::CDockAreaWidget* bottomArea = nullptr;
    ads::CDockAreaWidget* centerArea = nullptr;

    // Pass 1: Place the first panel per area to establish the dock areas
    for (const auto& def : PanelRegistry::instance().panels()) {
        auto* dw = m_dockWidgets.value(def.id);
        if (!dw) continue;

        switch (def.defaultArea) {
        case ads::LeftDockWidgetArea:
            if (!leftArea)
                leftArea = m_dockManager->addDockWidget(ads::LeftDockWidgetArea, dw);
            break;
        case ads::RightDockWidgetArea:
            if (!rightArea)
                rightArea = m_dockManager->addDockWidget(ads::RightDockWidgetArea, dw);
            break;
        case ads::BottomDockWidgetArea:
            if (!bottomArea)
                bottomArea = m_dockManager->addDockWidget(ads::BottomDockWidgetArea, dw);
            break;
        case ads::CenterDockWidgetArea:
        default:
            if (!centerArea)
                centerArea = m_dockManager->addDockWidget(ads::CenterDockWidgetArea, dw);
            break;
        }
    }

    m_centralArea = centerArea;

    // Pass 2: Tab remaining panels into their area
    for (const auto& def : PanelRegistry::instance().panels()) {
        auto* dw = m_dockWidgets.value(def.id);
        if (!dw || dw->dockAreaWidget()) continue; // already placed

        ads::CDockAreaWidget* targetArea = nullptr;
        switch (def.defaultArea) {
        case ads::LeftDockWidgetArea:   targetArea = leftArea;   break;
        case ads::RightDockWidgetArea:  targetArea = rightArea;  break;
        case ads::BottomDockWidgetArea: targetArea = bottomArea; break;
        default:                        targetArea = centerArea; break;
        }

        if (targetArea)
            m_dockManager->addDockWidgetTabToArea(dw, targetArea);
        else
            m_dockManager->addDockWidget(def.defaultArea, dw);
    }

    // Save this as the "Default" perspective
    m_dockManager->addPerspective("Default");
}

// ---------------------------------------------------------------------------
// Menus
// ---------------------------------------------------------------------------
void MainWindow::createMenus()
{
    // --- File menu ---
    auto* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("Save Layout", this, &MainWindow::saveDockState);
    fileMenu->addAction("Restore Layout", this, [this]() { restoreDockState(); });
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", QKeySequence::Quit, this, &QMainWindow::close);

    // --- View menu (auto-populated by DockManager) ---
    auto* viewMenu = menuBar()->addMenu("&View");

    // Add toggle actions grouped by category
    for (const auto& category : PanelRegistry::instance().categories()) {
        auto* catMenu = viewMenu->addMenu(category);
        for (const auto& def : PanelRegistry::instance().panelsInCategory(category)) {
            auto* dw = m_dockWidgets.value(def.id);
            if (dw)
                catMenu->addAction(dw->toggleViewAction());
        }
    }

    viewMenu->addSeparator();
    viewMenu->addAction("Show All Panels", this, [this]() {
        for (auto* dw : m_dockWidgets)
            dw->toggleView(true);
    });
    viewMenu->addAction("Hide All Panels", this, [this]() {
        for (auto* dw : m_dockWidgets)
            dw->toggleView(false);
    });

    // --- Perspective menu ---
    m_perspectiveMenu = menuBar()->addMenu("&Perspectives");
    rebuildPerspectiveMenu();

    // --- Help menu ---
    auto* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("About", this, [this]() {
        QMessageBox::about(this, "QtADS Master Template",
            QString("Qt Advanced Docking System Template\n\n"
                    "Panels registered: %1\n"
                    "Categories: %2")
                .arg(PanelRegistry::instance().panels().size())
                .arg(PanelRegistry::instance().categories().join(", ")));
    });
}

void MainWindow::rebuildPerspectiveMenu()
{
    m_perspectiveMenu->clear();

    m_perspectiveMenu->addAction("Save Perspective...", this, &MainWindow::savePerspective);
    m_perspectiveMenu->addSeparator();

    // Built-in perspectives
    m_perspectiveMenu->addAction("Default", m_dockManager, [this]() {
        m_dockManager->openPerspective("Default");
    });

    m_perspectiveMenu->addSeparator();

    // List any saved perspectives from settings
    QSettings settings;
    settings.beginGroup(kPerspGroup);
    m_dockManager->loadPerspectives(settings);
    settings.endGroup();

    for (const auto& name : m_dockManager->perspectiveNames()) {
        if (name == "Default") continue;
        m_perspectiveMenu->addAction(name, m_dockManager, [this, name]() {
            m_dockManager->openPerspective(name);
        });
    }
}

// ---------------------------------------------------------------------------
// Perspective & State management
// ---------------------------------------------------------------------------
void MainWindow::savePerspective()
{
    bool ok = false;
    QString name = QInputDialog::getText(this, "Save Perspective",
        "Perspective name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    m_dockManager->addPerspective(name);

    QSettings settings;
    settings.beginGroup(kPerspGroup);
    m_dockManager->savePerspectives(settings);
    settings.endGroup();

    rebuildPerspectiveMenu();

    statusBar()->showMessage(QString("Perspective '%1' saved").arg(name), 3000);
}

void MainWindow::saveDockState()
{
    QSettings settings;
    settings.setValue(kStateKey, m_dockManager->saveState());
    settings.setValue(kGeomKey, saveGeometry());
    statusBar()->showMessage("Layout saved", 3000);
}

bool MainWindow::restoreDockState()
{
    QSettings settings;
    auto state = settings.value(kStateKey).toByteArray();
    auto geom  = settings.value(kGeomKey).toByteArray();

    if (state.isEmpty())
        return false;

    if (!geom.isEmpty())
        restoreGeometry(geom);

    if (!m_dockManager->restoreState(state)) {
        qWarning() << "Failed to restore dock state, falling back to default layout";
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Close event: save state automatically
// ---------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent* event)
{
    saveDockState();
    QMainWindow::closeEvent(event);
}
