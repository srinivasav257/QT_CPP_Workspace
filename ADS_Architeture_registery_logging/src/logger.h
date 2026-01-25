#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QQueue>

// Log levels matching industry standards
enum class LogLevel {
    Trace,      // Very detailed, for debugging only
    Debug,      // Debugging information
    Info,       // General information
    Warning,    // Warning messages
    Error,      // Error messages
    Critical,   // Critical errors
    Fatal       // Fatal errors that cause termination
};

// Log categories for filtering
enum class LogCategory {
    System,         // System-level events
    CAN,            // CAN interface
    Serial,         // Serial communication
    PowerSupply,    // Power supply operations
    Oscilloscope,   // Oscilloscope operations
    DMM,            // Digital multimeter
    Modbus,         // Modbus relay
    TestExecution,  // Test execution engine
    Diagnostics,    // Diagnostic operations (DTC, DID)
    Trace,          // Trace monitoring
    Database,       // Database operations
    UI,             // User interface events
    FileIO,         // File operations
    Network,        // Network communication
    Script          // Script execution
};

// Single log entry structure
struct LogEntry {
    QDateTime timestamp;
    LogLevel level;
    LogCategory category;
    QString source;     // Component/class that generated the log
    QString message;
    QString context;    // Additional context (file, line, function)
    int threadId;

    LogEntry(LogLevel lvl, LogCategory cat, const QString& src, const QString& msg)
        : timestamp(QDateTime::currentDateTime())
        , level(lvl)
        , category(cat)
        , source(src)
        , message(msg)
        , threadId(0)
    {}

    QString toString() const;
    QString toHtml() const;
};

// Singleton Logger class
class Logger : public QObject
{
    Q_OBJECT

public:
    static Logger& instance();

    // Core logging functions
    void log(LogLevel level, LogCategory category, const QString& source,
             const QString& message, const QString& context = QString());

    // Convenience methods
    void trace(LogCategory cat, const QString& src, const QString& msg);
    void debug(LogCategory cat, const QString& src, const QString& msg);
    void info(LogCategory cat, const QString& src, const QString& msg);
    void warning(LogCategory cat, const QString& src, const QString& msg);
    void error(LogCategory cat, const QString& src, const QString& msg);
    void critical(LogCategory cat, const QString& src, const QString& msg);

    // Configuration
    void setLogLevel(LogLevel level);
    void setLogToFile(bool enable, const QString& filepath = QString());
    void setLogToConsole(bool enable);
    void enableCategory(LogCategory category, bool enable);
    void setMaxBufferSize(int size);

    // Retrieval
    QList<LogEntry> getRecentLogs(int count = 100);
    QList<LogEntry> getLogsByCategory(LogCategory category, int count = 100);
    QList<LogEntry> getLogsByLevel(LogLevel minLevel, int count = 100);

    // Utility
    void clear();
    void flush();
    QString levelToString(LogLevel level) const;
    QString categoryToString(LogCategory category) const;
    QColor levelToColor(LogLevel level) const;

signals:
    void logEntryAdded(const LogEntry& entry);
    void logsCleared();

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void writeToFile(const LogEntry& entry);
    void writeToConsole(const LogEntry& entry);
    bool shouldLog(LogLevel level, LogCategory category) const;

    // Configuration
    LogLevel m_minLevel;
    bool m_logToFile;
    bool m_logToConsole;
    QString m_logFilePath;
    int m_maxBufferSize;

    // State
    QFile m_logFile;
    QTextStream m_fileStream;
    QQueue<LogEntry> m_logBuffer;
    QMap<LogCategory, bool> m_enabledCategories;

    // Thread safety
    mutable QMutex m_mutex;
};

// Convenience macros for logging with automatic source info
#define LOG_TRACE(cat, msg) \
Logger::instance().trace(cat, QString("%1::%2").arg(__FILE__).arg(__LINE__), msg)

#define LOG_DEBUG(cat, msg) \
    Logger::instance().debug(cat, QString("%1::%2").arg(__FILE__).arg(__LINE__), msg)

#define LOG_INFO(cat, msg) \
    Logger::instance().info(cat, QString("%1::%2").arg(__FILE__).arg(__LINE__), msg)

#define LOG_WARNING(cat, msg) \
    Logger::instance().warning(cat, QString("%1::%2").arg(__FILE__).arg(__LINE__), msg)

#define LOG_ERROR(cat, msg) \
    Logger::instance().error(cat, QString("%1::%2").arg(__FILE__).arg(__LINE__), msg)

#define LOG_CRITICAL(cat, msg) \
    Logger::instance().critical(cat, QString("%1::%2").arg(__FILE__).arg(__LINE__), msg)
