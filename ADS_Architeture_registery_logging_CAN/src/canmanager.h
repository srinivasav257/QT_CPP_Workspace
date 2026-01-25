#pragma once

#include <QObject>
#include <QMap>
#include "caninterface.h"
#include "dbcparser.h"

class CANManager : public QObject
{
    Q_OBJECT

public:
    static CANManager& instance();

    // Interface management
    bool registerInterface(const QString& name, ICANInterface* interface);
    ICANInterface* getInterface(const QString& name);
    QStringList availableInterfaces() const;

    bool setActiveInterface(const QString& name);
    ICANInterface* activeInterface() const { return m_activeInterface; }
    QString activeInterfaceName() const { return m_activeInterfaceName; }

    // Database management
    bool loadDatabase(const QString& filepath);
    DBCDatabase* database() { return &m_database; }
    const DBCDatabase* database() const { return &m_database; }

    // Quick access to active interface operations
    bool openChannel(const CANChannelConfig& config);
    bool closeChannel();
    bool setBusActive(bool active);
    bool transmit(const CANMessage& msg);
    bool transmit(uint32_t id, const uint8_t* data, uint8_t dlc);
    bool transmitSignal(uint32_t msgId, const QString& signalName, const QVariant& value);

    // Message building helpers
    CANMessage buildMessage(uint32_t id, const QMap<QString, QVariant>& signalValues);
    QMap<QString, QVariant> parseMessage(const CANMessage& msg);

    // Status
    bool isConnected() const;
    bool isBusActive() const;
    CANStatistics getStatistics() const;

signals:
    void interfaceChanged(const QString& name);
    void databaseLoaded(const QString& filepath);
    void messageReceived(const CANMessage& msg);
    void messageParsed(uint32_t id, const QMap<QString, QVariant>& signals);
    void errorOccurred(const QString& error);

private:
    CANManager();
    ~CANManager();
    CANManager(const CANManager&) = delete;
    CANManager& operator=(const CANManager&) = delete;

    void onMessageReceived(const CANMessage& msg);

    QMap<QString, ICANInterface*> m_interfaces;
    ICANInterface* m_activeInterface;
    QString m_activeInterfaceName;
    DBCDatabase m_database;
};

    LOG_INFO(LogCategory::CAN,
             QString("Active interface set to: %1").arg(name));

    emit interfaceChanged(name);
    return true;
}

bool CANManager::loadDatabase(const QString& filepath)
{
    if (!m_database.loadFromFile(filepath)) {
        LOG_ERROR(LogCategory::CAN,
                  QString("Failed to load DBC: %1").arg(m_database.getLastError()));
        return false;
    }

    LOG_INFO(LogCategory::CAN,
             QString("DBC loaded: %1 (%2 messages)")
                 .arg(filepath)
                 .arg(m_database.messages().size()));

    emit databaseLoaded(filepath);
    return true;
}

bool CANManager::openChannel(const CANChannelConfig& config)
{
    if (!m_activeInterface) {
        LOG_ERROR(LogCategory::CAN, "No active interface");
        return false;
    }

    return m_activeInterface->open(config);
}

bool CANManager::closeChannel()
{
    if (!m_activeInterface) {
        return false;
    }

    return m_activeInterface->close();
}

bool CANManager::setBusActive(bool active)
{
    if (!m_activeInterface) {
        LOG_ERROR(LogCategory::CAN, "No active interface");
        return false;
    }

    return m_activeInterface->setBusActive(active);
}

bool CANManager::transmit(const CANMessage& msg)
{
    if (!m_activeInterface) {
        LOG_ERROR(LogCategory::CAN, "No active interface");
        return false;
    }

    return m_activeInterface->transmit(msg);
}

bool CANManager::transmit(uint32_t id, const uint8_t* data, uint8_t dlc)
{
    CANMessage msg;
    msg.id = id;
    msg.dlc = dlc;
    memcpy(msg.data, data, dlc);

    return transmit(msg);
}

bool CANManager::transmitSignal(uint32_t msgId, const QString& signalName, const QVariant& value)
{
    const DBCMessage* dbcMsg = m_database.getMessage(msgId);
    if (!dbcMsg) {
        LOG_ERROR(LogCategory::CAN,
                  QString("Message not found in DBC: 0x%1").arg(msgId, 0, 16));
        return false;
    }

    CANMessage msg;
    msg.id = msgId;
    msg.dlc = dbcMsg->dlc;
    memset(msg.data, 0, sizeof(msg.data));

    QMap<QString, QVariant> values;
    values[signalName] = value;

    dbcMsg->encodeData(msg.data, values);

    return transmit(msg);
}

CANMessage CANManager::buildMessage(uint32_t id, const QMap<QString, QVariant>& signalValues)
{
    CANMessage msg;
    msg.id = id;

    const DBCMessage* dbcMsg = m_database.getMessage(id);
    if (dbcMsg) {
        msg.dlc = dbcMsg->dlc;
        memset(msg.data, 0, sizeof(msg.data));
        dbcMsg->encodeData(msg.data, signalValues);
    } else {
        msg.dlc = 8;
        memset(msg.data, 0, sizeof(msg.data));
    }

    return msg;
}

QMap<QString, QVariant> CANManager::parseMessage(const CANMessage& msg)
{
    const DBCMessage* dbcMsg = m_database.getMessage(msg.id);
    if (!dbcMsg) {
        return QMap<QString, QVariant>();
    }

    return dbcMsg->parseData(msg.data, msg.dlc);
}

bool CANManager::isConnected() const
{
    return m_activeInterface && m_activeInterface->isOpen();
}

bool CANManager::isBusActive() const
{
    return m_activeInterface && m_activeInterface->isBusActive();
}

CANStatistics CANManager::getStatistics() const
{
    if (m_activeInterface) {
        return m_activeInterface->getStatistics();
    }
    return CANStatistics();
}

void CANManager::onMessageReceived(const CANMessage& msg)
{
    emit messageReceived(msg);

    // Parse if we have database
    if (m_database.isLoaded()) {
        QMap<QString, QVariant> signals = parseMessage(msg);
        if (!signals.isEmpty()) {
            emit messageParsed(msg.id, signals);
        }
    }
}

// ============================================================================
// Usage Example in main application
// ============================================================================

/*
// In your MainWindow or initialization code:

void MainWindow::initializeCANSystem()
{
    CANManager& canMgr = CANManager::instance();

    // Register simulation interface
    auto* simInterface = new CANSimulationInterface(this);
    simInterface->initialize();
    canMgr.registerInterface("Simulation", simInterface);

    // Register Vector XL interface (if available)
    auto* vectorInterface = new CANVectorXLInterface(this);
    if (vectorInterface->isAvailable()) {
        vectorInterface->initialize();
        canMgr.registerInterface("Vector XL", vectorInterface);
    } else {
        delete vectorInterface;
    }

    // Set active interface
    canMgr.setActiveInterface("Simulation");

    // Load DBC database
    canMgr.loadDatabase("C:/CANDatabases/my_vehicle.dbc");

    // Configure and open channel
    CANChannelConfig config;
    config.channel = 0;
    config.baudrate = 500000;
    config.fdEnabled = false;

    if (canMgr.openChannel(config)) {
        canMgr.setBusActive(true);
        LOG_INFO(LogCategory::CAN, "CAN system initialized successfully");
    }

    // Create trace window
    m_canTraceWindow = new CANTraceWindow();
    m_canTraceWindow->setCANInterface(canMgr.activeInterface());
    m_canTraceWindow->setDatabase(canMgr.database());

    // Add to dock
    DockConfig traceConfig = DockWidgetFactory::getConfig(DockType::CANMessages);
    traceConfig.widgetFactory = [this]() -> QWidget* {
        return m_canTraceWindow;
    };

    ads::CDockWidget* traceDock = DockWidgetFactory::createDock(traceConfig);
    m_appDockManager->dockManager()->addDockWidget(ads::CenterDockWidgetArea, traceDock);
}

// Sending a message with signals
void sendEngineRPM(double rpm)
{
    CANManager& canMgr = CANManager::instance();

    QMap<QString, QVariant> signals;
    signals["EngineSpeed"] = rpm;
    signals["EngineRunning"] = true;

    CANMessage msg = canMgr.buildMessage(0x200, signals);
    canMgr.transmit(msg);
}

// Or send a single signal value
void setGearPosition(int gear)
{
    CANManager::instance().transmitSignal(0x300, "GearPosition", gear);
}

// Setup periodic messages in simulation
void setupSimulationMessages()
{
    auto* simInterface = qobject_cast<CANSimulationInterface*>(
        CANManager::instance().getInterface("Simulation")
    );

    if (simInterface) {
        // Periodic engine RPM message (every 10ms)
        CANMessage engineMsg;
        engineMsg.id = 0x200;
        engineMsg.dlc = 8;
        simInterface->addPeriodicMessage(engineMsg, 10);

        // Periodic vehicle speed (every 20ms)
        CANMessage speedMsg;
        speedMsg.id = 0x201;
        speedMsg.dlc = 8;
        simInterface->addPeriodicMessage(speedMsg, 20);
    }
}

// Receive and decode messages
connect(&CANManager::instance(), &CANManager::messageParsed,
        this, [](uint32_t id, const QMap<QString, QVariant>& signals) {

    if (id == 0x200) { // Engine message
        double rpm = signals.value("EngineSpeed").toDouble();
        bool running = signals.value("EngineRunning").toBool();

        qDebug() << "Engine RPM:" << rpm << "Running:" << running;
    }
});
*/
