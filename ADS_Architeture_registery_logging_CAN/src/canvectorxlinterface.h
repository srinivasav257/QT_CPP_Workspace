// canvectorxlinterface.h
#pragma once

#include "caninterface.h"
#include <QThread>

// Forward declarations for Vector XL API types
// You'll need to include actual vxlapi.h when integrating
typedef unsigned long XLhandle;
typedef unsigned long XLaccess;
typedef unsigned long long XLuint64;

class VectorXLReceiveThread;

class CANVectorXLInterface : public ICANInterface
{
    Q_OBJECT

public:
    explicit CANVectorXLInterface(QObject* parent = nullptr);
    ~CANVectorXLInterface() override;

    // ICANInterface implementation
    CANInterfaceType type() const override { return CANInterfaceType::VectorXL; }
    QString name() const override { return "Vector XL"; }
    QString description() const override { return "Vector XL API CAN Interface"; }
    bool isAvailable() const override;

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

    // Vector-specific
    QStringList enumerateDevices();
    bool selectDevice(const QString& deviceName);
    QString getDriverVersion() const;

private:
    friend class VectorXLReceiveThread;

    bool loadVectorXLLibrary();
    void unloadVectorXLLibrary();
    bool openDriver();
    bool closeDriver();
    bool setChannelParams();

    // Vector XL handles
    XLhandle m_portHandle;
    XLaccess m_channelMask;
    XLaccess m_permissionMask;

    QString m_selectedDevice;
    VectorXLReceiveThread* m_receiveThread;
    bool m_libraryLoaded;
};
