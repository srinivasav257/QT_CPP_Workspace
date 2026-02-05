#include "WorkspaceManager.h"
#include "DockManager.h"
#include "DockWidget.h"

#include <QSettings>
#include <QDebug>

namespace DockManager {

static const char* kStateKey     = "DockManager/State";
static const char* kGeometryKey  = "DockManager/Geometry";
static const char* kPerspGroup   = "Perspectives";
static const char* kLockedKey    = "DockManager/Locked";

struct WorkspaceManager::Private
{
    ads::CDockManager* dockManager = nullptr;
    QString currentPerspective;
    bool locked = false;
};

WorkspaceManager::WorkspaceManager(ads::CDockManager* dockManager, QObject* parent)
    : QObject(parent)
    , d(new Private)
{
    d->dockManager = dockManager;
}

WorkspaceManager::~WorkspaceManager() = default;

// --- Perspective Management ---

void WorkspaceManager::savePerspective(const QString& name)
{
    if (!d->dockManager || name.isEmpty())
        return;

    d->dockManager->addPerspective(name);
    d->currentPerspective = name;

    // Persist to settings
    savePerspectives();

    emit perspectiveSaved(name);
}

bool WorkspaceManager::loadPerspective(const QString& name)
{
    if (!d->dockManager)
        return false;

    if (!d->dockManager->perspectiveNames().contains(name)) {
        qWarning() << "WorkspaceManager: perspective not found:" << name;
        return false;
    }

    d->dockManager->openPerspective(name);
    d->currentPerspective = name;

    emit perspectiveChanged(name);
    return true;
}

void WorkspaceManager::removePerspective(const QString& name)
{
    if (!d->dockManager)
        return;

    d->dockManager->removePerspective(name);
    savePerspectives();

    if (d->currentPerspective == name)
        d->currentPerspective.clear();
}

QStringList WorkspaceManager::perspectiveNames() const
{
    if (!d->dockManager)
        return {};

    return d->dockManager->perspectiveNames();
}

QString WorkspaceManager::currentPerspective() const
{
    return d->currentPerspective;
}

// --- State Persistence ---

void WorkspaceManager::saveState()
{
    if (!d->dockManager)
        return;

    QSettings settings;
    settings.setValue(kStateKey, d->dockManager->saveState());
    settings.setValue(kLockedKey, d->locked);
}

bool WorkspaceManager::restoreState()
{
    if (!d->dockManager)
        return false;

    QSettings settings;
    auto state = settings.value(kStateKey).toByteArray();

    if (state.isEmpty())
        return false;

    bool success = d->dockManager->restoreState(state);
    if (!success) {
        qWarning() << "WorkspaceManager: failed to restore dock state";
    }

    // Restore lock state
    d->locked = settings.value(kLockedKey, false).toBool();
    if (d->locked) {
        setLocked(true);
    }

    return success;
}

void WorkspaceManager::savePerspectives()
{
    if (!d->dockManager)
        return;

    QSettings settings;
    settings.beginGroup(kPerspGroup);
    d->dockManager->savePerspectives(settings);
    settings.endGroup();
}

void WorkspaceManager::loadPerspectives()
{
    if (!d->dockManager)
        return;

    QSettings settings;
    settings.beginGroup(kPerspGroup);
    d->dockManager->loadPerspectives(settings);
    settings.endGroup();
}

void WorkspaceManager::saveGeometry(const QByteArray& geometry)
{
    QSettings settings;
    settings.setValue(kGeometryKey, geometry);
}

QByteArray WorkspaceManager::savedGeometry() const
{
    QSettings settings;
    return settings.value(kGeometryKey).toByteArray();
}

// --- Workspace Locking ---

bool WorkspaceManager::isLocked() const
{
    return d->locked;
}

void WorkspaceManager::setLocked(bool locked)
{
    if (d->locked == locked)
        return;

    d->locked = locked;

    if (!d->dockManager) {
        emit lockedChanged(locked);
        return;
    }

    // Lock/unlock all dock widget features
    auto features = locked
        ? ads::CDockWidget::NoDockWidgetFeatures
        : ads::CDockWidget::DefaultDockWidgetFeatures;

    d->dockManager->lockDockWidgetFeaturesGlobally(features);

    emit lockedChanged(locked);
}

} // namespace DockManager
