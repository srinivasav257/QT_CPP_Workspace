// cansimulationinterface.h
#pragma once

#include "caninterface.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QRandomGenerator>

class CANSimulationInterface : public ICANInterface
{
    Q_OBJECT

public:
    explicit CANSimulationInterface(QObject* parent = nullptr);
    ~CANSimulationInterface() override;

    // ICANInterface implementation
    CANInterfaceType type() const override { return CANInterfaceType::Simulation; }
    QString name() const override { return "Simulation"; }
    QString description() const override { return "Virtual CAN Interface for Testing"; }
    bool isAvailable() const override { return true; }

    bool initialize() override;
    bool open(const CANChannelConfig& config) override;
    bool close() override;
    bool isOpen() const override { return m_isOpen; }

    bool transmit(const CANMessage& msg) override;
    bool transmitBurst(const QVector<CANMessage>& messages) override;

    bool setChannelConfig(const CANChannelConfig& config) override;
    CANChannelConfig getChannelConfig() const override { return m_config; }
    bool setBusActive(bool active) override;
    bool isBusActive() const override { return m_busActive; }

    CANStatistics getStatistics() const override { return m_statistics; }
    void resetStatistics() override;
    QString getLastError() const override { return m_lastError; }

    // Simulation-specific features
    void enableAutoResponse(bool enable) { m_autoResponseEnabled = enable; }
    void setSimulationSpeed(double speed) { m_simulationSpeed = speed; }
    void addPeriodicMessage(const CANMessage& msg, int periodMs);
    void removePeriodicMessage(uint32_t id);
    void injectMessage(const CANMessage& msg);
    void simulateBusError();

private slots:
    void onPeriodicTimer();
    void onStatisticsTimer();

private:
    void startTimers();
    void stopTimers();
    CANMessage generateAutoResponse(const CANMessage& request);
    uint64_t getCurrentTimestamp();

    QTimer* m_periodicTimer;
    QTimer* m_statisticsTimer;
    QElapsedTimer m_elapsedTimer;

    struct PeriodicMessage {
        CANMessage message;
        int periodMs;
        qint64 lastSent;
    };

    QMap<uint32_t, PeriodicMessage> m_periodicMessages;
    bool m_autoResponseEnabled;
    double m_simulationSpeed;
};
