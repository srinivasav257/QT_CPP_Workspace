#include "logger.h"
#include <QThread>
#include <QDebug>
#include <QDir>
#include <QColor>

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_minLevel(LogLevel::Debug)
    , m_logToFile(false)
    , m_logToConsole(true)
    , m_maxBufferSize(10000)
{
    // Enable all categories by default
    for (int i = 0; i <= static_cast<int>(LogCategory::Script); ++i) {
        m_enabledCategories[static_cast<LogCategory>(i)] = true;
    }
}

Logger::~Logger()
{
    flush();
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

void Logger::log(LogLevel level, LogCategory category, const QString& source,
                 const QString& message, const QString& context)
{
    if (!shouldLog(level, category)) {
        return;
    }

    QMutexLocker locker(&m_mutex);

    LogEntry entry(level, category, source, message);
    entry.context = context;
    entry.threadId = static_cast<int>(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    // Add to buffer
    m_logBuffer.enqueue(entry);

    // Maintain max buffer size
    while (m_logBuffer.size() > m_maxBufferSize) {
        m_logBuffer.dequeue();
    }

    // Write to outputs
    if (m_logToFile) {
        writeToFile(entry);
    }

    if (m_logToConsole) {
        writeToConsole(entry);
    }

    // Emit signal for UI updates
    emit logEntryAdded(entry);
}

void Logger::trace(LogCategory cat, const QString& src, const QString& msg)
{
    log(LogLevel::Trace, cat, src, msg);
}

void Logger::debug(LogCategory cat, const QString& src, const QString& msg)
{
    log(LogLevel::Debug, cat, src, msg);
}

void Logger::info(LogCategory cat, const QString& src, const QString& msg)
{
    log(LogLevel::Info, cat, src, msg);
}

void Logger::warning(LogCategory cat, const QString& src, const QString& msg)
{
    log(LogLevel::Warning, cat, src, msg);
}

void Logger::error(LogCategory cat, const QString& src, const QString& msg)
{
    log(LogLevel::Error, cat, src, msg);
}

void Logger::critical(LogCategory cat, const QString& src, const QString& msg)
{
    log(LogLevel::Critical, cat, src, msg);
}

void Logger::setLogLevel(LogLevel level)
{
    QMutexLocker locker(&m_mutex);
    m_minLevel = level;
}

void Logger::setLogToFile(bool enable, const QString& filepath)
{
    QMutexLocker locker(&m_mutex);

    if (m_logFile.isOpen()) {
        m_fileStream.flush();
        m_logFile.close();
    }

    m_logToFile = enable;

    if (enable) {
        QString path = filepath;
        if (path.isEmpty()) {
            // Default: logs in application directory with timestamp
            QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
            path = QString("logs/spyder_log_%1.txt").arg(timestamp);
        }

        // Ensure directory exists
        QFileInfo fileInfo(path);
        QDir dir = fileInfo.absoluteDir();
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        m_logFilePath = path;
        m_logFile.setFileName(path);

        if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            m_fileStream.setDevice(&m_logFile);
            m_fileStream << "=== Log started at "
                         << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
                         << " ===" << Qt::endl;
        } else {
            qWarning() << "Failed to open log file:" << path;
            m_logToFile = false;
        }
    }
}

void Logger::setLogToConsole(bool enable)
{
    QMutexLocker locker(&m_mutex);
    m_logToConsole = enable;
}

void Logger::enableCategory(LogCategory category, bool enable)
{
    QMutexLocker locker(&m_mutex);
    m_enabledCategories[category] = enable;
}

void Logger::setMaxBufferSize(int size)
{
    QMutexLocker locker(&m_mutex);
    m_maxBufferSize = size;

    while (m_logBuffer.size() > m_maxBufferSize) {
        m_logBuffer.dequeue();
    }
}

QList<LogEntry> Logger::getRecentLogs(int count)
{
    QMutexLocker locker(&m_mutex);

    QList<LogEntry> result;
    int start = qMax(0, m_logBuffer.size() - count);

    for (int i = start; i < m_logBuffer.size(); ++i) {
        result.append(m_logBuffer[i]);
    }

    return result;
}

QList<LogEntry> Logger::getLogsByCategory(LogCategory category, int count)
{
    QMutexLocker locker(&m_mutex);

    QList<LogEntry> result;
    for (int i = m_logBuffer.size() - 1; i >= 0 && result.size() < count; --i) {
        if (m_logBuffer[i].category == category) {
            result.prepend(m_logBuffer[i]);
        }
    }

    return result;
}

QList<LogEntry> Logger::getLogsByLevel(LogLevel minLevel, int count)
{
    QMutexLocker locker(&m_mutex);

    QList<LogEntry> result;
    for (int i = m_logBuffer.size() - 1; i >= 0 && result.size() < count; --i) {
        if (m_logBuffer[i].level >= minLevel) {
            result.prepend(m_logBuffer[i]);
        }
    }

    return result;
}

void Logger::clear()
{
    QMutexLocker locker(&m_mutex);
    m_logBuffer.clear();
    emit logsCleared();
}

void Logger::flush()
{
    QMutexLocker locker(&m_mutex);
    if (m_logFile.isOpen()) {
        m_fileStream.flush();
    }
}

QString Logger::levelToString(LogLevel level) const
{
    switch (level) {
    case LogLevel::Trace:    return "TRACE";
    case LogLevel::Debug:    return "DEBUG";
    case LogLevel::Info:     return "INFO";
    case LogLevel::Warning:  return "WARNING";
    case LogLevel::Error:    return "ERROR";
    case LogLevel::Critical: return "CRITICAL";
    case LogLevel::Fatal:    return "FATAL";
    default:                 return "UNKNOWN";
    }
}

QString Logger::categoryToString(LogCategory category) const
{
    switch (category) {
    case LogCategory::System:        return "System";
    case LogCategory::CAN:           return "CAN";
    case LogCategory::Serial:        return "Serial";
    case LogCategory::PowerSupply:   return "PowerSupply";
    case LogCategory::Oscilloscope:  return "Oscilloscope";
    case LogCategory::DMM:           return "DMM";
    case LogCategory::Modbus:        return "Modbus";
    case LogCategory::TestExecution: return "TestExec";
    case LogCategory::Diagnostics:   return "Diagnostics";
    case LogCategory::Trace:         return "Trace";
    case LogCategory::Database:      return "Database";
    case LogCategory::UI:            return "UI";
    case LogCategory::FileIO:        return "FileIO";
    case LogCategory::Network:       return "Network";
    case LogCategory::Script:        return "Script";
    default:                         return "Unknown";
    }
}

QColor Logger::levelToColor(LogLevel level) const
{
    switch (level) {
    case LogLevel::Trace:    return QColor(150, 150, 150);  // Gray
    case LogLevel::Debug:    return QColor(100, 149, 237);  // Cornflower Blue
    case LogLevel::Info:     return QColor(60, 179, 113);   // Medium Sea Green
    case LogLevel::Warning:  return QColor(255, 165, 0);    // Orange
    case LogLevel::Error:    return QColor(220, 20, 60);    // Crimson
    case LogLevel::Critical: return QColor(139, 0, 0);      // Dark Red
    case LogLevel::Fatal:    return QColor(75, 0, 130);     // Indigo
    default:                 return QColor(0, 0, 0);        // Black
    }
}

bool Logger::shouldLog(LogLevel level, LogCategory category) const
{
    return (level >= m_minLevel) && m_enabledCategories.value(category, true);
}

void Logger::writeToFile(const LogEntry& entry)
{
    if (!m_logFile.isOpen()) {
        return;
    }

    m_fileStream << entry.toString() << Qt::endl;
}

void Logger::writeToConsole(const LogEntry& entry)
{
    // Use qDebug for console output
    qDebug().noquote() << entry.toString();
}

// LogEntry implementations
QString LogEntry::toString() const
{
    return QString("[%1] [%2] [%3] %4: %5")
    .arg(timestamp.toString("yyyy-MM-dd HH:mm:ss.zzz"))
        .arg(Logger::instance().levelToString(level), -8)
        .arg(Logger::instance().categoryToString(category), -12)
        .arg(source)
        .arg(message);
}

QString LogEntry::toHtml() const
{
    QColor color = Logger::instance().levelToColor(level);

    return QString("<span style='color: %1;'>[%2] [%3] [%4] %5: %6</span>")
        .arg(color.name())
        .arg(timestamp.toString("HH:mm:ss.zzz"))
        .arg(Logger::instance().levelToString(level))
        .arg(Logger::instance().categoryToString(category))
        .arg(source)
        .arg(message.toHtmlEscaped());
}
