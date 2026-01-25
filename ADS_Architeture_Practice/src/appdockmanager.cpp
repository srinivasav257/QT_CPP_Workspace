#include "appdockmanager.h"
#include <QLabel>
#include <QTextEdit>
#include <QSettings>

// Fixed: Use proper QWidget parent, not QObject
AppDockManager::AppDockManager(QWidget *parent)
    : QObject(parent)
{
    // Create DockManager with the parent widget directly
    m_adsManager = new ads::CDockManager(parent);
}

AppDockManager::~AppDockManager()
{
    // Cleanup is handled by Qt parent-child relationship
    // but good to be explicit about the order
    m_docks.clear();
}

ads::CDockManager *AppDockManager::dockManager() const
{
    return m_adsManager;
}

void AppDockManager::createProjectDock()
{
    auto *widget = new QLabel("Project Explorer");
    widget->setAlignment(Qt::AlignCenter);

    auto *dock = new ads::CDockWidget("Project");
    dock->setObjectName("dock.project");
    dock->setWidget(widget);

    m_adsManager->addDockWidget(ads::LeftDockWidgetArea, dock);
    dock->toggleView(false); // Use toggleView instead of closeDockWidget

    m_docks.insert(dock->objectName(), dock);

    connect(dock, &ads::CDockWidget::viewToggled,
            this, [this](bool visible)
            {
                if (visible) {
                    emit dockActivated();
                } });
}

void AppDockManager::createCanMessagesDock()
{
    auto *widget = new QLabel("CAN Messages View");
    widget->setAlignment(Qt::AlignCenter);

    auto *dock = new ads::CDockWidget("CAN Messages");
    dock->setObjectName("dock.can_messages");
    dock->setWidget(widget);

    m_adsManager->addDockWidget(ads::CenterDockWidgetArea, dock);
    dock->toggleView(false);

    m_docks.insert(dock->objectName(), dock);

    connect(dock, &ads::CDockWidget::viewToggled,
            this, [this](bool visible)
            {
                if (visible) {
                    emit dockActivated();
                } });
}

void AppDockManager::createPropertiesPanelDock()
{
    auto *widget = new QLabel("Properties Panel");
    widget->setAlignment(Qt::AlignCenter);

    auto *dock = new ads::CDockWidget("Properties");
    dock->setObjectName("dock.properties");
    dock->setWidget(widget);

    m_adsManager->addDockWidget(ads::RightDockWidgetArea, dock);
    dock->toggleView(false);

    m_docks.insert(dock->objectName(), dock);

    connect(dock, &ads::CDockWidget::viewToggled,
            this, [this](bool visible)
            {
                if (visible) {
                    emit dockActivated();
                } });
}

void AppDockManager::createLogDock()
{
    auto *widget = new QLabel("Log Output");
    widget->setAlignment(Qt::AlignCenter);

    auto *dock = new ads::CDockWidget("Log");
    dock->setObjectName("dock.log");
    dock->setWidget(widget);

    m_adsManager->addDockWidget(ads::BottomDockWidgetArea, dock);
    dock->toggleView(false);

    m_docks.insert(dock->objectName(), dock);

    connect(dock, &ads::CDockWidget::viewToggled,
            this, [this](bool visible)
            {
                if (visible) {
                    emit dockActivated();
                } });
}

QList<QAction *> AppDockManager::viewMenuActions()
{
    QList<QAction *> actions;

    for (ads::CDockWidget *dock : m_docks)
    {
        if (dock)
        {
            QAction *action = dock->toggleViewAction();
            action->setText(dock->windowTitle());
            actions.append(action);
        }
    }

    return actions;
}

bool AppDockManager::hasSavedLayout() const
{
    QSettings settings("SPYDER", "AutoTraceTool");
    return settings.contains("layout/main");
}

void AppDockManager::saveLayout()
{
    if (!m_adsManager)
    {
        qWarning() << "Cannot save layout: DockManager is null";
        return;
    }

    QByteArray state = m_adsManager->saveState();
    qDebug() << "Saving layout, bytes:" << state.size();

    if (state.isEmpty())
    {
        qWarning() << "Layout state is empty, not saving";
        return;
    }

    // FIXED: Use consistent organization/app name
    QSettings settings("SPYDER", "AutoTraceTool");
    settings.setValue("layout/main", state);
    settings.sync(); // Ensure it's written to disk

    qDebug() << "Layout saved successfully";
}

void AppDockManager::restoreLayout()
{
    if (!m_adsManager)
    {
        qWarning() << "Cannot restore layout: DockManager is null";
        return;
    }

    qDebug() << "Restoring layout, docks known:" << m_docks.keys();

    QSettings settings("SPYDER", "AutoTraceTool");

    if (!settings.contains("layout/main"))
    {
        qDebug() << "No saved layout found";
        return;
    }

    QByteArray state = settings.value("layout/main").toByteArray();

    if (state.isEmpty())
    {
        qWarning() << "Saved layout is empty";
        return;
    }

    m_adsManager->restoreState(state);
    qDebug() << "Layout restored successfully";
}
