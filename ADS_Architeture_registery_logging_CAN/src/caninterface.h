// caninterface.h - Abstract base class for all CAN hardware interfaces
#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <cstdint>

// CAN Message structure
struct CANMessage {
    uint32_t id;                    // CAN identifier
    uint8_t data[8];                // Data bytes (max 8 for CAN 2.0)
    uint8_t dlc;                    // Data length code (0-8)
    bool isExtended;                // Extended ID (29-bit) vs Standard (11-bit)
    bool isRTR;                     // Remote Transmission Request
    bool isFD;                      // CAN FD flag
    uint64_t timestamp;             // Timestamp in microseconds
    uint8_t channel;                // CAN channel number (0 or 1 for HS/FD)

    CANMessage()
        : id(0), dlc(0), isExtended(false), isRTR(false),
        isFD(false), timestamp(0), channel(0)
    {
        memset(data, 0, sizeof(data));
    }

    QString toString() const;
    QString dataToHex() const;
};

// CAN channel configuration
struct CANChannelConfig {
    uint8_t channel;                // Channel number
    uint32_t baudrate;              // Baudrate in bps (e.g., 500000 for 500kbps)
    uint32_t dataBaudrate;          // Data baudrate for CAN FD
    bool fdEnabled;                 // Enable CAN FD
    bool listenOnly;                // Listen-only mode (no ACK)

    CANChannelConfig()
        : channel(0), baudrate(500000), dataBaudrate(2000000),
        fdEnabled(false), listenOnly(false)
    {}
};

// CAN statistics
struct CANStatistics {
    uint64_t txCount;               // Transmitted messages
    uint64_t rxCount;               // Received messages
    uint64_t errorCount;            // Error frames
    uint32_t busLoad;               // Bus load percentage (0-100)
    uint32_t txErrors;              // TX error counter
    uint32_t rxErrors;              // RX error counter

    CANStatistics()
        : txCount(0), rxCount(0), errorCount(0),
        busLoad(0), txErrors(0), rxErrors(0)
    {}

    void reset() {
        txCount = rxCount = errorCount = 0;
        busLoad = txErrors = rxErrors = 0;
    }
};

// Hardware interface types
enum class CANInterfaceType {
    Simulation,     // Software simulation
    VectorXL,       // Vector XL API
    PeakCAN,        // Peak PCAN
    Kvaser,         // Kvaser
    SocketCAN,      // Linux SocketCAN
    Custom          // Custom driver
};

// Abstract CAN interface
class ICANInterface : public QObject
{
    Q_OBJECT

public:
    virtual ~ICANInterface() = default;

    // Interface information
    virtual CANInterfaceType type() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual bool isAvailable() const = 0;

    // Lifecycle
    virtual bool initialize() = 0;
    virtual bool open(const CANChannelConfig& config) = 0;
    virtual bool close() = 0;
    virtual bool isOpen() const = 0;

    // Communication
    virtual bool transmit(const CANMessage& msg) = 0;
    virtual bool transmitBurst(const QVector<CANMessage>& messages) = 0;

    // Configuration
    virtual bool setChannelConfig(const CANChannelConfig& config) = 0;
    virtual CANChannelConfig getChannelConfig() const = 0;
    virtual bool setBusActive(bool active) = 0;
    virtual bool isBusActive() const = 0;

    // Status and statistics
    virtual CANStatistics getStatistics() const = 0;
    virtual void resetStatistics() = 0;
    virtual QString getLastError() const = 0;

signals:
    void messageReceived(const CANMessage& msg);
    void errorOccurred(const QString& error);
    void busStateChanged(bool active);
    void statisticsUpdated(const CANStatistics& stats);

protected:
    QString m_lastError;
    CANChannelConfig m_config;
    CANStatistics m_statistics;
    bool m_isOpen = false;
    bool m_busActive = false;
};

// Helper functions
inline QString CANMessage::toString() const
{
    QString result = QString("ID: 0x%1 [%2] DLC: %3 Data: %4")
    .arg(id, isExtended ? 8 : 3, 16, QChar('0'))
        .arg(isExtended ? "Ext" : "Std")
        .arg(dlc)
        .arg(dataToHex());

    if (isRTR) result += " [RTR]";
    if (isFD) result += " [FD]";

    return result;
}

inline QString CANMessage::dataToHex() const
{
    QString result;
    for (int i = 0; i < dlc; ++i) {
        result += QString("%1 ").arg(data[i], 2, 16, QChar('0')).toUpper();
    }
    return result.trimmed();
}
