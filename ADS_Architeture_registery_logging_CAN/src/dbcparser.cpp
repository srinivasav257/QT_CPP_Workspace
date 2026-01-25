// dbcparser.cpp
#include "dbcparser.h"
#include "logger.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

DBCDatabase::DBCDatabase()
    : m_loaded(false)
    , m_currentMessageId(0)
{
}

bool DBCDatabase::loadFromFile(const QString& filepath)
{
    clear();

    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = QString("Cannot open file: %1").arg(filepath);
        LOG_ERROR(LogCategory::CAN, m_lastError);
        return false;
    }

    LOG_INFO(LogCategory::CAN, QString("Loading DBC file: %1").arg(filepath));

    QTextStream stream(&file);
    int lineNumber = 0;

    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();
        lineNumber++;

        if (line.isEmpty() || line.startsWith("//")) {
            continue;
        }

        if (!parseLine(line)) {
            LOG_WARNING(LogCategory::CAN,
                        QString("Parse warning at line %1: %2").arg(lineNumber).arg(line));
        }
    }

    file.close();
    m_loaded = true;

    LOG_INFO(LogCategory::CAN,
             QString("DBC loaded: %1 messages, %2 ECUs")
                 .arg(m_messages.size())
                 .arg(m_ecus.size()));

    return true;
}

bool DBCDatabase::saveToFile(const QString& filepath)
{
    // Implement DBC file writing if needed
    // This is a complex task, typically not needed for read-only applications
    return false;
}

void DBCDatabase::clear()
{
    m_messages.clear();
    m_ecus.clear();
    m_version.clear();
    m_description.clear();
    m_lastError.clear();
    m_loaded = false;
}

DBCMessage* DBCDatabase::getMessage(uint32_t id)
{
    auto it = m_messages.find(id);
    return (it != m_messages.end()) ? &it.value() : nullptr;
}

const DBCMessage* DBCDatabase::getMessage(uint32_t id) const
{
    auto it = m_messages.find(id);
    return (it != m_messages.end()) ? &it.value() : nullptr;
}

DBCMessage* DBCDatabase::getMessage(const QString& name)
{
    for (auto& msg : m_messages) {
        if (msg.name == name) {
            return &msg;
        }
    }
    return nullptr;
}

const DBCMessage* DBCDatabase::getMessage(const QString& name) const
{
    for (const auto& msg : m_messages) {
        if (msg.name == name) {
            return &msg;
        }
    }
    return nullptr;
}

DBCSignal* DBCDatabase::getSignal(uint32_t messageId, const QString& signalName)
{
    DBCMessage* msg = getMessage(messageId);
    if (!msg) return nullptr;

    auto it = msg->signals.find(signalName);
    return (it != msg->signals.end()) ? &it.value() : nullptr;
}

const DBCSignal* DBCDatabase::getSignal(uint32_t messageId, const QString& signalName) const
{
    const DBCMessage* msg = getMessage(messageId);
    if (!msg) return nullptr;

    auto it = msg->signals.find(signalName);
    return (it != msg->signals.end()) ? &it.value() : nullptr;
}

QStringList DBCDatabase::getMessageNames() const
{
    QStringList names;
    for (const auto& msg : m_messages) {
        names << msg.name;
    }
    return names;
}

QStringList DBCDatabase::getSignalNames(uint32_t messageId) const
{
    const DBCMessage* msg = getMessage(messageId);
    return msg ? msg->signals.keys() : QStringList();
}

bool DBCDatabase::parseLine(const QString& line)
{
    // Version
    if (line.startsWith("VERSION")) {
        QRegularExpression re("VERSION \"(.+)\"");
        auto match = re.match(line);
        if (match.hasMatch()) {
            m_version = match.captured(1);
        }
        return true;
    }

    // Message definition: BO_ <ID> <Name>: <DLC> <Sender>
    if (line.startsWith("BO_ ")) {
        return parseMessage(line);
    }

    // Signal definition: SG_ <Name> : <StartBit>|<Length>@<Endian><Sign> (<Factor>,<Offset>) [<Min>|<Max>] "<Unit>" <Receivers>
    if (line.startsWith(" SG_ ")) {
        return parseSignal(line);
    }

    // Comments: CM_ [SG_|BO_] <ID> [<SignalName>] "<Comment>";
    if (line.startsWith("CM_ ")) {
        return parseComment(line);
    }

    // Value tables: VAL_ <ID> <SignalName> <Value> "<Description>" ... ;
    if (line.startsWith("VAL_ ")) {
        return parseValueTable(line);
    }

    // Attributes: BA_ "<AttributeName>" [BO_|SG_] <ID> [<SignalName>] <Value>;
    if (line.startsWith("BA_ ")) {
        return parseAttribute(line);
    }

    return true;
}

bool DBCDatabase::parseMessage(const QString& line)
{
    // BO_ 123 MessageName: 8 ECU_Name
    QRegularExpression re(R"(BO_ (\d+) (\w+): (\d+) (\w+))");
    auto match = re.match(line);

    if (!match.hasMatch()) {
        return false;
    }

    DBCMessage msg;
    msg.id = match.captured(1).toUInt();
    msg.name = match.captured(2);
    msg.dlc = match.captured(3).toUInt();
    msg.sender = match.captured(4);

    m_messages[msg.id] = msg;
    m_currentMessageId = msg.id;

    return true;
}

bool DBCDatabase::parseSignal(const QString& line)
{
    // SG_ SignalName : 0|8@1+ (1,0) [0|255] "unit" ECU1,ECU2
    QRegularExpression re(
        R"( SG_ (\w+) : (\d+)\|(\d+)@([01])([+-]) \(([^,]+),([^)]+)\) \[([^|]+)\|([^\]]+)\] \"([^\"]*)\" (.+))"
        );
    auto match = re.match(line);

    if (!match.hasMatch() || m_currentMessageId == 0) {
        return false;
    }

    DBCSignal signal;
    signal.name = match.captured(1);
    signal.startBit = match.captured(2).toUInt();
    signal.bitLength = match.captured(3).toUInt();
    signal.isLittleEndian = (match.captured(4) == "1");
    signal.isSigned = (match.captured(5) == "-");
    signal.factor = match.captured(6).toDouble();
    signal.offset = match.captured(7).toDouble();
    signal.minimum = match.captured(8).toDouble();
    signal.maximum = match.captured(9).toDouble();
    signal.unit = match.captured(10);

    QString receivers = match.captured(11);
    signal.receivers = receivers.split(',', Qt::SkipEmptyParts);

    m_messages[m_currentMessageId].signals[signal.name] = signal;

    return true;
}

bool DBCDatabase::parseComment(const QString& line)
{
    // CM_ SG_ 123 SignalName "Comment";
    // CM_ BO_ 123 "Message Comment";

    if (line.contains("SG_")) {
        QRegularExpression re(R"(CM_ SG_ (\d+) (\w+) \"([^\"]*)\")");
        auto match = re.match(line);
        if (match.hasMatch()) {
            uint32_t msgId = match.captured(1).toUInt();
            QString sigName = match.captured(2);
            QString comment = match.captured(3);

            DBCSignal* sig = getSignal(msgId, sigName);
            if (sig) {
                sig->comment = comment;
            }
        }
    } else if (line.contains("BO_")) {
        QRegularExpression re(R"(CM_ BO_ (\d+) \"([^\"]*)\")");
        auto match = re.match(line);
        if (match.hasMatch()) {
            uint32_t msgId = match.captured(1).toUInt();
            QString comment = match.captured(2);

            DBCMessage* msg = getMessage(msgId);
            if (msg) {
                msg->comment = comment;
            }
        }
    }

    return true;
}

bool DBCDatabase::parseValueTable(const QString& line)
{
    // VAL_ 123 SignalName 0 "Off" 1 "On" 2 "Error" ;
    QRegularExpression re(R"(VAL_ (\d+) (\w+) (.*);)");
    auto match = re.match(line);

    if (!match.hasMatch()) {
        return false;
    }

    uint32_t msgId = match.captured(1).toUInt();
    QString sigName = match.captured(2);
    QString valuesStr = match.captured(3);

    DBCSignal* sig = getSignal(msgId, sigName);
    if (!sig) {
        return false;
    }

    // Parse value-description pairs
    QRegularExpression pairRe(R"((\d+) \"([^\"]*)\")");
    auto it = pairRe.globalMatch(valuesStr);

    while (it.hasNext()) {
        auto pairMatch = it.next();
        int value = pairMatch.captured(1).toInt();
        QString desc = pairMatch.captured(2);
        sig->valueTable[value] = desc;
    }

    return true;
}

bool DBCDatabase::parseAttribute(const QString& line)
{
    // BA_ "GenMsgCycleTime" BO_ 123 100;
    QRegularExpression re(R"(BA_ \"GenMsgCycleTime\" BO_ (\d+) (\d+))");
    auto match = re.match(line);

    if (match.hasMatch()) {
        uint32_t msgId = match.captured(1).toUInt();
        uint32_t cycleTime = match.captured(2).toUInt();

        DBCMessage* msg = getMessage(msgId);
        if (msg) {
            msg->cycleTime = cycleTime;
        }
        return true;
    }

    return true; // Ignore unknown attributes
}

QVariant DBCSignal::extractValue(const uint8_t* data, uint8_t dlc) const
{
    // Extract raw bits
    uint64_t rawValue = 0;

    if (isLittleEndian) {
        // Intel (little endian) byte order
        uint8_t bytePos = startBit / 8;
        uint8_t bitPos = startBit % 8;

        for (uint8_t i = 0; i < bitLength; ++i) {
            if (bytePos >= dlc) break;

            if (data[bytePos] & (1 << bitPos)) {
                rawValue |= (1ULL << i);
            }

            bitPos++;
            if (bitPos == 8) {
                bitPos = 0;
                bytePos++;
            }
        }
    } else {
        // Motorola (big endian) byte order
        int bitPos = startBit;

        for (uint8_t i = 0; i < bitLength; ++i) {
            uint8_t bytePos = bitPos / 8;
            uint8_t bit = 7 - (bitPos % 8);

            if (bytePos >= dlc) break;

            if (data[bytePos] & (1 << bit)) {
                rawValue |= (1ULL << (bitLength - 1 - i));
            }

            bitPos++;
        }
    }

    // Handle signed values
    if (isSigned && (rawValue & (1ULL << (bitLength - 1)))) {
        // Sign extend
        rawValue |= (~0ULL << bitLength);
    }

    // Convert to physical value
    double physical = toPhysical(rawValue);

    // Return as appropriate type
    if (!valueTable.isEmpty()) {
        return valueTable.value(static_cast<int>(rawValue), QString::number(rawValue));
    }

    return physical;
}

void DBCSignal::encodeValue(uint8_t* data, const QVariant& value) const
{
    double physical = value.toDouble();
    uint64_t rawValue = toRaw(physical);

    // Mask to bit length
    uint64_t mask = (1ULL << bitLength) - 1;
    rawValue &= mask;

    if (isLittleEndian) {
        // Intel byte order
        uint8_t bytePos = startBit / 8;
        uint8_t bitPos = startBit % 8;

        for (uint8_t i = 0; i < bitLength; ++i) {
            if (rawValue & (1ULL << i)) {
                data[bytePos] |= (1 << bitPos);
            } else {
                data[bytePos] &= ~(1 << bitPos);
            }

            bitPos++;
            if (bitPos == 8) {
                bitPos = 0;
                bytePos++;
            }
        }
    } else {
        // Motorola byte order
        int bitPos = startBit;

        for (uint8_t i = 0; i < bitLength; ++i) {
            uint8_t bytePos = bitPos / 8;
            uint8_t bit = 7 - (bitPos % 8);

            if (rawValue & (1ULL << (bitLength - 1 - i))) {
                data[bytePos] |= (1 << bit);
            } else {
                data[bytePos] &= ~(1 << bit);
            }

            bitPos++;
        }
    }
}

QMap<QString, QVariant> DBCMessage::parseData(const uint8_t* data, uint8_t msgDlc) const
{
    QMap<QString, QVariant> result;

    for (auto it = signals.begin(); it != signals.end(); ++it) {
        const DBCSignal& sig = it.value();
        result[sig.name] = sig.extractValue(data, msgDlc);
    }

    return result;
}

void DBCMessage::encodeData(uint8_t* data, const QMap<QString, QVariant>& values) const
{
    // Initialize to zero
    memset(data, 0, dlc);

    for (auto it = values.begin(); it != values.end(); ++it) {
        const QString& sigName = it.key();
        const QVariant& value = it.value();

        auto sigIt = signals.find(sigName);
        if (sigIt != signals.end()) {
            sigIt.value().encodeValue(data, value);
        }
    }
}
