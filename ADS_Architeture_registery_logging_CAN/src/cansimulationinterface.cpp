// cansimulationinterface.cpp
#include "cansimulationinterface.h"
#include "logger.h"
#include <QDateTime>

CANSimulationInterface::CANSimulationInterface(QObject* parent)
    : ICANInterface()
    , m_autoResponseEnabled(false)
    , m_simulationSpeed(1.0)
{
    setParent(parent);

    m_periodicTimer = new QTimer(this);
    m_periodicTimer->setInterval(10); // Check every 10ms
    connect(m_periodicTimer, &QTimer::timeout, this, &CANSimulationInterface::onPeriodicTimer);

    m_statisticsTimer = new QTimer(this);
    m_statisticsTimer->setInterval(1000); // Update stats every second
    connect(m_statisticsTimer, &QTimer::timeout, this, &CANSimulationInterface::onStatisticsTimer);
}

CANSimulationInterface::~CANSimulationInterface()
{
    if (m_isOpen) {
        close();
    }
}

bool CANSimulationInterface::initialize()
{
    LOG_INFO(LogCategory::CAN, "Simulation interface initialized");
    return true;
}

bool CANSimulationInterface::open(const CANChannelConfig& config)
{
    if (m_isOpen) {
        m_lastError = "Interface already open";
        LOG_WARNING(LogCategory::CAN, m_lastError);
        return false;
    }

    m_config = config;
    m_isOpen = true;
    m_elapsedTimer.start();

    LOG_INFO(LogCategory::CAN,
             QString("Simulation interface opened - Ch:%1, Baudrate:%2, FD:%3")
                 .arg(config.channel)
                 .arg(config.baudrate)
                 .arg(config.fdEnabled ? "Yes" : "No"));

    return true;
}

bool CANSimulationInterface::close()
{
    if (!m_isOpen) {
        return true;
    }

    stopTimers();
    m_periodicMessages.clear();
    m_isOpen = false;
    m_busActive = false;

    LOG_INFO(LogCategory::CAN, "Simulation interface closed");
    emit busStateChanged(false);

    return true;
}

bool CANSimulationInterface::transmit(const CANMessage& msg)
{
    if (!m_isOpen) {
        m_lastError = "Interface not open";
        return false;
    }

    if (!m_busActive) {
        m_lastError = "Bus not active";
        return false;
    }

    // Simulate transmission
    m_statistics.txCount++;

    LOG_TRACE(LogCategory::CAN,
              QString("TX [Ch%1]: %2").arg(msg.channel).arg(msg.toString()));

    // Simulate echo (loopback)
    CANMessage echo = msg;
    echo.timestamp = getCurrentTimestamp();

    QTimer::singleShot(1, this, [this, echo]() {
        emit messageReceived(echo);
    });

    // Auto-response simulation
    if (m_autoResponseEnabled) {
        CANMessage response = generateAutoResponse(msg);
        QTimer::singleShot(5, this, [this, response]() {
            emit messageReceived(response);
            m_statistics.rxCount++;
        });
    }

    return true;
}

bool CANSimulationInterface::transmitBurst(const QVector<CANMessage>& messages)
{
    bool success = true;
    for (const CANMessage& msg : messages) {
        if (!transmit(msg)) {
            success = false;
        }
    }
    return success;
}

bool CANSimulationInterface::setChannelConfig(const CANChannelConfig& config)
{
    m_config = config;

    LOG_DEBUG(LogCategory::CAN,
              QString("Config updated - Baudrate:%1, FD:%2")
                  .arg(config.baudrate)
                  .arg(config.fdEnabled ? "Yes" : "No"));

    return true;
}

bool CANSimulationInterface::setBusActive(bool active)
{
    if (!m_isOpen) {
        m_lastError = "Interface not open";
        return false;
    }

    m_busActive = active;

    if (active) {
        startTimers();
        LOG_INFO(LogCategory::CAN, "Bus activated");
    } else {
        stopTimers();
        LOG_INFO(LogCategory::CAN, "Bus deactivated");
    }

    emit busStateChanged(active);
    return true;
}

void CANSimulationInterface::resetStatistics()
{
    m_statistics.reset();
    LOG_DEBUG(LogCategory::CAN, "Statistics reset");
}

void CANSimulationInterface::addPeriodicMessage(const CANMessage& msg, int periodMs)
{
    PeriodicMessage periodic;
    periodic.message = msg;
    periodic.periodMs = periodMs;
    periodic.lastSent = 0;

    m_periodicMessages[msg.id] = periodic;

    LOG_DEBUG(LogCategory::CAN,
              QString("Added periodic message ID:0x%1, Period:%2ms")
                  .arg(msg.id, 0, 16)
                  .arg(periodMs));
}

void CANSimulationInterface::removePeriodicMessage(uint32_t id)
{
    m_periodicMessages.remove(id);

    LOG_DEBUG(LogCategory::CAN,
              QString("Removed periodic message ID:0x%1").arg(id, 0, 16));
}

void CANSimulationInterface::injectMessage(const CANMessage& msg)
{
    if (!m_isOpen || !m_busActive) {
        return;
    }

    CANMessage injected = msg;
    injected.timestamp = getCurrentTimestamp();

    m_statistics.rxCount++;
    emit messageReceived(injected);

    LOG_TRACE(LogCategory::CAN,
              QString("Injected message: %1").arg(injected.toString()));
}

void CANSimulationInterface::simulateBusError()
{
    m_statistics.errorCount++;

    QString error = "Simulated bus error";
    LOG_WARNING(LogCategory::CAN, error);
    emit errorOccurred(error);
}

void CANSimulationInterface::onPeriodicTimer()
{
    if (!m_busActive) {
        return;
    }

    qint64 currentTime = m_elapsedTimer.elapsed();

    for (auto it = m_periodicMessages.begin(); it != m_periodicMessages.end(); ++it) {
        PeriodicMessage& periodic = it.value();

        if (currentTime - periodic.lastSent >= periodic.periodMs) {
            CANMessage msg = periodic.message;
            msg.timestamp = getCurrentTimestamp();
            msg.channel = m_config.channel;

            // Simulate some data variation
            if (msg.dlc > 0) {
                msg.data[0] = (msg.data[0] + 1) % 256;
            }

            m_statistics.rxCount++;
            emit messageReceived(msg);

            periodic.lastSent = currentTime;
        }
    }
}

void CANSimulationInterface::onStatisticsTimer()
{
    // Simulate bus load (random 10-40%)
    m_statistics.busLoad = QRandomGenerator::global()->bounded(10, 40);

    emit statisticsUpdated(m_statistics);
}

void CANSimulationInterface::startTimers()
{
    m_periodicTimer->start();
    m_statisticsTimer->start();
    m_elapsedTimer.restart();
}

void CANSimulationInterface::stopTimers()
{
    m_periodicTimer->stop();
    m_statisticsTimer->stop();
}

CANMessage CANSimulationInterface::generateAutoResponse(const CANMessage& request)
{
    CANMessage response;
    response.id = request.id + 0x10; // Response offset
    response.dlc = request.dlc;
    response.isExtended = request.isExtended;
    response.isFD = request.isFD;
    response.channel = request.channel;
    response.timestamp = getCurrentTimestamp();

    // Echo data back with slight modification
    for (int i = 0; i < request.dlc; ++i) {
        response.data[i] = ~request.data[i]; // Invert bits
    }

    return response;
}

uint64_t CANSimulationInterface::getCurrentTimestamp()
{
    return static_cast<uint64_t>(m_elapsedTimer.nsecsElapsed() / 1000); // microseconds
}
