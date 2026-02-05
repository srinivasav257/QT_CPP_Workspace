#include "DockToolBar.h"
#include "WorkspaceManager.h"

#include <QAction>
#include <QInputDialog>

namespace DockManager {

struct DockToolBar::Private
{
    WorkspaceManager* workspaceManager = nullptr;

    QAction* saveAction = nullptr;
    QAction* restoreAction = nullptr;
    QAction* lockAction = nullptr;
    QAction* createPerspectiveAction = nullptr;
};

DockToolBar::DockToolBar(WorkspaceManager* workspaceManager, QWidget* parent)
    : QToolBar("Workspace", parent)
    , d(new Private)
{
    d->workspaceManager = workspaceManager;

    setObjectName("DockToolBar");

    // --- Save/Restore Actions ---
    d->saveAction = addAction("Save Layout");
    d->saveAction->setToolTip("Save the current dock layout");
    connect(d->saveAction, &QAction::triggered, this, [this]() {
        if (d->workspaceManager)
            d->workspaceManager->saveState();
    });

    d->restoreAction = addAction("Restore Layout");
    d->restoreAction->setToolTip("Restore the saved dock layout");
    connect(d->restoreAction, &QAction::triggered, this, [this]() {
        if (d->workspaceManager)
            d->workspaceManager->restoreState();
    });

    addSeparator();

    // --- Perspective Controls ---
    d->createPerspectiveAction = addAction("+");
    d->createPerspectiveAction->setToolTip("Save current layout as a new perspective");
    connect(d->createPerspectiveAction, &QAction::triggered,
            this, &DockToolBar::onCreatePerspective);

    addSeparator();

    // --- Lock Action ---
    d->lockAction = addAction("Lock");
    d->lockAction->setCheckable(true);
    d->lockAction->setToolTip("Lock the workspace to prevent layout changes");
    connect(d->lockAction, &QAction::toggled, this, [this](bool checked) {
        if (d->workspaceManager)
            d->workspaceManager->setLocked(checked);
    });

    // Connect to workspace manager signals
    if (d->workspaceManager) {
        connect(d->workspaceManager, &WorkspaceManager::lockedChanged,
                this, &DockToolBar::onLockedChanged);
    }
}

DockToolBar::~DockToolBar() = default;

// --- Visibility Control ---

void DockToolBar::setSaveRestoreVisible(bool visible)
{
    d->saveAction->setVisible(visible);
    d->restoreAction->setVisible(visible);
}

void DockToolBar::setPerspectivesVisible(bool visible)
{
    d->createPerspectiveAction->setVisible(visible);
}

void DockToolBar::setLockVisible(bool visible)
{
    d->lockAction->setVisible(visible);
}

// --- Action Access ---

QAction* DockToolBar::saveAction() const
{
    return d->saveAction;
}

QAction* DockToolBar::restoreAction() const
{
    return d->restoreAction;
}

QAction* DockToolBar::lockAction() const
{
    return d->lockAction;
}

QAction* DockToolBar::createPerspectiveAction() const
{
    return d->createPerspectiveAction;
}

// --- Slots ---

void DockToolBar::onCreatePerspective()
{
    if (!d->workspaceManager)
        return;

    bool ok = false;
    QString name = QInputDialog::getText(
        this,
        tr("Save Perspective"),
        tr("Perspective name:"),
        QLineEdit::Normal,
        QString(),
        &ok);

    if (ok && !name.isEmpty()) {
        d->workspaceManager->savePerspective(name);
    }
}

void DockToolBar::onLockedChanged(bool locked)
{
    d->lockAction->blockSignals(true);
    d->lockAction->setChecked(locked);
    d->lockAction->setText(locked ? "Unlock" : "Lock");
    d->lockAction->blockSignals(false);
}

} // namespace DockManager
