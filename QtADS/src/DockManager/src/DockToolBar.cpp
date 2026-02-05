#include "DockToolBar.h"
#include "WorkspaceManager.h"

#include <QAction>
#include <QComboBox>
#include <QInputDialog>
#include <QIcon>

namespace DockManager {

struct DockToolBar::Private
{
    WorkspaceManager* workspaceManager = nullptr;

    QAction* saveAction = nullptr;
    QAction* restoreAction = nullptr;
    QAction* lockAction = nullptr;
    QAction* createPerspectiveAction = nullptr;
    QComboBox* perspectiveCombo = nullptr;
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
    d->perspectiveCombo = new QComboBox(this);
    d->perspectiveCombo->setToolTip("Select a perspective");
    d->perspectiveCombo->setMinimumWidth(120);
    addWidget(d->perspectiveCombo);

    connect(d->perspectiveCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DockToolBar::onPerspectiveSelected);

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
        connect(d->workspaceManager, &WorkspaceManager::perspectiveSaved,
                this, &DockToolBar::updatePerspectiveList);
        connect(d->workspaceManager, &WorkspaceManager::perspectiveChanged,
                this, [this](const QString& name) {
                    int index = d->perspectiveCombo->findText(name);
                    if (index >= 0) {
                        d->perspectiveCombo->blockSignals(true);
                        d->perspectiveCombo->setCurrentIndex(index);
                        d->perspectiveCombo->blockSignals(false);
                    }
                });
    }

    // Initial perspective list
    updatePerspectiveList();
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
    d->perspectiveCombo->setVisible(visible);
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

QComboBox* DockToolBar::perspectiveComboBox() const
{
    return d->perspectiveCombo;
}

// --- Slots ---

void DockToolBar::updatePerspectiveList()
{
    if (!d->workspaceManager)
        return;

    d->perspectiveCombo->blockSignals(true);
    QString current = d->perspectiveCombo->currentText();
    d->perspectiveCombo->clear();
    d->perspectiveCombo->addItems(d->workspaceManager->perspectiveNames());

    int index = d->perspectiveCombo->findText(current);
    if (index >= 0)
        d->perspectiveCombo->setCurrentIndex(index);

    d->perspectiveCombo->blockSignals(false);
}

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

void DockToolBar::onPerspectiveSelected(int index)
{
    if (!d->workspaceManager || index < 0)
        return;

    QString name = d->perspectiveCombo->itemText(index);
    d->workspaceManager->loadPerspective(name);
}

void DockToolBar::onLockedChanged(bool locked)
{
    d->lockAction->blockSignals(true);
    d->lockAction->setChecked(locked);
    d->lockAction->setText(locked ? "Unlock" : "Lock");
    d->lockAction->blockSignals(false);
}

} // namespace DockManager
