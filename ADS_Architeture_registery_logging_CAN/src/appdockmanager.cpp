#include "appdockmanager.h"
#include "logger.h"
#include "logviewerwidget.h"
#include <QSettings>
#include <QDebug>

AppDockManager::AppDockManager(QWidget* parent)
    : QObject(parent)
    , m_anyDockOpened(false)
{
    m_adsManager = new ads::CDockManager(parent);

    Logger::instance().info(LogCategory::System, "AppDockManager",
                            "Dock manager initialized");
}

AppDockManager::~AppDockManager()
{
    m_docks.clear();
    m_docksByName.clear();
}

ads::CDockManager* AppDockManager::dockManager() const
{
    return m_adsManager;
}

void AppDockManager::createDock(DockType type)
{
    if (m_docks.contains(type)) {
        Logger::instance().warning(LogCategory::System, "AppDockManager",
                                   QString("Dock already exists: %1").arg(static_cast<int>(type)));
        return;
    }

    DockConfig config = DockWidgetFactory::getConfig(type);

    // Special handling for Log dock - use custom widget
    if (type == DockType::Log) {
        config.widgetFactory = []() -> QWidget* {
            return new LogViewerWidget();
        };
    }

    ads::CDockWidget* dock = DockWidgetFactory::createDock(config);

    if (!dock) {
        Logger::instance().error(LogCategory::System, "AppDockManager",
                                 QString("Failed to create dock: %1").arg(static_cast<int>(type)));
        return;
    }

    // Add to dock manager
    m_adsManager->addDockWidget(config.defaultArea, dock);

    // Set initial visibility
    dock->toggleView(config.startVisible);

    // Register the dock
    registerDock(type, dock);

    // Connect visibility signal
    connect(dock, &ads::CDockWidget::viewToggled,
            this, [this, type](bool visible) {
                onDockViewToggled(type, visible);
            });

    Logger::instance().debug(LogCategory::System, "AppDockManager",
                             QString("Created dock: %1 - %2")
                                 .arg(static_cast<int>(type))
                                 .arg(config.title));
}

void AppDockManager::createAllDocks()
{
    Logger::instance().info(LogCategory::System, "AppDockManager",
                            "Creating all docks...");

    // Create docks in specific order for better layout
    QList<DockType> dockOrder = {
        // Essential docks first
        DockType::Log,
        DockType::TestSequencer,
        DockType::ProjectExplorer,
        DockType::Properties,

        // CAN interface
        DockType::CANMessages,
        DockType::CANConfiguration,
        DockType::CANDatabase,
        DockType::CANSignalMonitor,

        // Serial ports
        DockType::SerialPort1,
        DockType::SerialPort2,
        DockType::SerialPort3,
        DockType::SerialPort4,
        DockType::SerialTerminal,

        // Instruments
        DockType::PowerSupply,
        DockType::Oscilloscope,
        DockType::DMM,
        DockType::ModbusRelay,

        // Test execution
        DockType::TestResults,
        DockType::TestVariables,
        DockType::TestReport,

        // Diagnostics
        DockType::DTCMonitor,
        DockType::DIDReader,
        DockType::TraceMonitor,

        // Additional
        DockType::ScriptEditor,
        DockType::DataRecorder,
        DockType::SystemMonitor
    };

    for (DockType type : dockOrder) {
        createDock(type);
    }

    Logger::instance().info(LogCategory::System, "AppDockManager",
                            QString("Created %1 docks").arg(m_docks.size()));
}

ads::CDockWidget* AppDockManager::getDock(DockType type) const
{
    return m_docks.value(type, nullptr);
}

void AppDockManager::showDock(DockType type)
{
    ads::CDockWidget* dock = getDock(type);
    if (dock) {
        dock->toggleView(true);
    }
}

void AppDockManager::hideDock(DockType type)
{
    ads::CDockWidget* dock = getDock(type);
    if (dock) {
        dock->toggleView(false);
    }
}

bool AppDockManager::isDockVisible(DockType type) const
{
    ads::CDockWidget* dock = getDock(type);
    return dock ? !dock->isClosed() : false;
}

QList<QAction*> AppDockManager::viewMenuActions()
{
    QList<QAction*> actions;

    for (auto it = m_docks.begin(); it != m_docks.end(); ++it) {
        ads::CDockWidget* dock = it.value();
        if (dock) {
            QAction* action = dock->toggleViewAction();
            action->setText(dock->windowTitle());
            actions.append(action);
        }
    }

    return actions;
}

QMap<QString, QList<QAction*>> AppDockManager::getGroupedMenuActions()
{
    QMap<QString, QList<QAction*>> groups;

    // Define groups
    QMap<DockType, QString> typeToGroup = {
        {DockType::ProjectExplorer, "Core"},
        {DockType::Properties, "Core"},
        {DockType::Log, "Core"},

        {DockType::CANMessages, "CAN Interface"},
        {DockType::CANConfiguration, "CAN Interface"},
        {DockType::CANDatabase, "CAN Interface"},
        {DockType::CANSignalMonitor, "CAN Interface"},

        {DockType::SerialPort1, "Serial Ports"},
        {DockType::SerialPort2, "Serial Ports"},
        {DockType::SerialPort3, "Serial Ports"},
        {DockType::SerialPort4, "Serial Ports"},
        {DockType::SerialTerminal, "Serial Ports"},

        {DockType::PowerSupply, "Instruments"},
        {DockType::Oscilloscope, "Instruments"},
        {DockType::DMM, "Instruments"},
        {DockType::ModbusRelay, "Instruments"},

        {DockType::TestSequencer, "Test Execution"},
        {DockType::TestResults, "Test Execution"},
        {DockType::TestVariables, "Test Execution"},
        {DockType::TestReport, "Test Execution"},

        {DockType::DTCMonitor, "Diagnostics"},
        {DockType::DIDReader, "Diagnostics"},
        {DockType::TraceMonitor, "Diagnostics"},

        {DockType::ScriptEditor, "Additional"},
        {DockType::DataRecorder, "Additional"},
        {DockType::SystemMonitor, "Additional"}
    };

    for (auto it = m_docks.begin(); it != m_docks.end(); ++it) {
        DockType type = it.key();
        ads::CDockWidget* dock = it.value();

        if (!dock) continue;

        QString group = typeToGroup.value(type, "Other");
        QAction* action = dock->toggleViewAction();
        action->setText(dock->windowTitle());

        groups[group].append(action);
    }

    return groups;
}

bool AppDockManager::hasSavedLayout() const
{
    QSettings settings("SPYDER", "AutoTraceTool");
    return settings.contains("layout/main");
}

void AppDockManager::saveLayout()
{
    if (!m_adsManager) {
        Logger::instance().warning(LogCategory::System, "AppDockManager",
                                   "Cannot save layout: DockManager is null");
        return;
    }

    QByteArray state = m_adsManager->saveState();

    if (state.isEmpty()) {
        Logger::instance().warning(LogCategory::System, "AppDockManager",
                                   "Layout state is empty, not saving");
        return;
    }

    QSettings settings("SPYDER", "AutoTraceTool");
    settings.setValue("layout/main", state);
    settings.sync();

    Logger::instance().info(LogCategory::System, "AppDockManager",
                            QString("Layout saved successfully (%1 bytes)").arg(state.size()));
}

void AppDockManager::restoreLayout()
{
    if (!m_adsManager) {
        Logger::instance().warning(LogCategory::System, "AppDockManager",
                                   "Cannot restore layout: DockManager is null");
        return;
    }

    QSettings settings("SPYDER", "AutoTraceTool");

    if (!settings.contains("layout/main")) {
        Logger::instance().debug(LogCategory::System, "AppDockManager",
                                 "No saved layout found");
        return;
    }

    QByteArray state = settings.value("layout/main").toByteArray();

    if (state.isEmpty()) {
        Logger::instance().warning(LogCategory::System, "AppDockManager",
                                   "Saved layout is empty");
        return;
    }

    m_adsManager->restoreState(state);
    Logger::instance().info(LogCategory::System, "AppDockManager",
                            "Layout restored successfully");
}

void AppDockManager::loadDefaultLayout()
{
    // Show essential docks by default
    showDock(DockType::Log);
    showDock(DockType::TestSequencer);
    showDock(DockType::ProjectExplorer);

    Logger::instance().info(LogCategory::System, "AppDockManager",
                            "Default layout loaded");
}

void AppDockManager::onDockViewToggled(DockType type, bool visible)
{
    if (visible) {
        emit dockActivated(type);

        if (!m_anyDockOpened) {
            m_anyDockOpened = true;
            emit firstDockOpened();
        }

        Logger::instance().debug(LogCategory::UI, "AppDockManager",
                                 QString("Dock opened: %1").arg(static_cast<int>(type)));
    }
}

void AppDockManager::registerDock(DockType type, ads::CDockWidget* dock)
{
    m_docks[type] = dock;
    m_docksByName[dock->objectName()] = dock;
}
