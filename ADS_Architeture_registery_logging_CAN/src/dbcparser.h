// dbcparser.h - DBC Database Parser
#pragma once

#include <QString>
#include <QMap>
#include <QVector>
#include <QVariant>

// Signal definition
struct DBCSignal {
    QString name;
    uint8_t startBit;
    uint8_t bitLength;
    bool isLittleEndian;        // 1 = Intel (little), 0 = Motorola (big)
    bool isSigned;
    double factor;
    double offset;
    double minimum;
    double maximum;
    QString unit;
    QStringList receivers;
    QString comment;

    // Value tables (for enum-like signals)
    QMap<int, QString> valueTable;

    DBCSignal()
        : startBit(0), bitLength(1), isLittleEndian(true), isSigned(false),
        factor(1.0), offset(0.0), minimum(0.0), maximum(0.0)
    {}

    // Extract signal value from CAN message data
    QVariant extractValue(const uint8_t* data, uint8_t dlc) const;

    // Encode value into CAN message data
    void encodeValue(uint8_t* data, const QVariant& value) const;

    // Get physical value from raw
    double toPhysical(uint64_t raw) const {
        return raw * factor + offset;
    }

    // Get raw value from physical
    uint64_t toRaw(double physical) const {
        return static_cast<uint64_t>((physical - offset) / factor);
    }
};

// Message definition
struct DBCMessage {
    uint32_t id;
    QString name;
    uint8_t dlc;
    QString sender;
    uint32_t cycleTime;         // in ms (0 = event-based)
    QString comment;
    QMap<QString, DBCSignal> signals;

    DBCMessage()
        : id(0), dlc(8), cycleTime(0)
    {}

    // Parse all signals from message data
    QMap<QString, QVariant> parseData(const uint8_t* data, uint8_t msgDlc) const;

    // Encode all signals into message data
    void encodeData(uint8_t* data, const QMap<QString, QVariant>& values) const;
};

// ECU (Electronic Control Unit) definition
struct DBCECU {
    QString name;
    QString comment;
    QStringList transmittedMessages;
    QStringList receivedMessages;
};

// Complete DBC Database
class DBCDatabase
{
public:
    DBCDatabase();

    bool loadFromFile(const QString& filepath);
    bool saveToFile(const QString& filepath);
    void clear();

    // Accessors
    const QMap<uint32_t, DBCMessage>& messages() const { return m_messages; }
    const QMap<QString, DBCECU>& ecus() const { return m_ecus; }

    DBCMessage* getMessage(uint32_t id);
    DBCMessage* getMessage(const QString& name);
    const DBCMessage* getMessage(uint32_t id) const;
    const DBCMessage* getMessage(const QString& name) const;

    DBCSignal* getSignal(uint32_t messageId, const QString& signalName);
    const DBCSignal* getSignal(uint32_t messageId, const QString& signalName) const;

    QStringList getMessageNames() const;
    QStringList getSignalNames(uint32_t messageId) const;

    QString getVersion() const { return m_version; }
    QString getDescription() const { return m_description; }

    bool isLoaded() const { return m_loaded; }
    QString getLastError() const { return m_lastError; }

private:
    bool parseLine(const QString& line);
    bool parseMessage(const QString& line);
    bool parseSignal(const QString& line);
    bool parseComment(const QString& line);
    bool parseValueTable(const QString& line);
    bool parseAttribute(const QString& line);

    uint64_t extractBits(const uint8_t* data, uint8_t startBit,
                         uint8_t bitLength, bool isLittleEndian) const;
    void insertBits(uint8_t* data, uint8_t startBit, uint8_t bitLength,
                    bool isLittleEndian, uint64_t value) const;

    QMap<uint32_t, DBCMessage> m_messages;
    QMap<QString, DBCECU> m_ecus;

    QString m_version;
    QString m_description;
    QString m_lastError;
    bool m_loaded;

    // Temporary storage during parsing
    uint32_t m_currentMessageId;
};
