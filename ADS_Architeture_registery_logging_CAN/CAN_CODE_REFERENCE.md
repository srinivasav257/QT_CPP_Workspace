# CAN Integration - Code Reference

## Modified Code Snippets

### 1. MainWindow Header Changes

**File:** `src/mainwindow.h`

```cpp
#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenu>

class AppDockManager;
class CANMessage;  // Forward declaration

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onFirstDockOpened();
    void onCANMessageReceived(const CANMessage& msg);        // NEW
    void onCANMessageParsed(uint32_t id, const QMap<QString, QVariant>& signals);  // NEW
    void onCANError(const QString& error);                   // NEW

private:
    // UI Components
    QMenu* m_viewMenu = nullptr;
    QStackedWidget* m_centralStack = nullptr;
    QWidget* m_welcomePage = nullptr;

    // Dock Management
    AppDockManager* m_appDockManager = nullptr;

    // Initialization methods
    void initializeDockSystem();
    void initializeCANSystem();        // NEW
    void createMenus();
    void createViewMenu();
    void createHelpMenu();
    void createToolsMenu();
    void createFileMenu();
    void createWelcomePage();

    // View switching
    void showWelcomePage();
    void showDockLayout();
};
```

### 2. MainWindow Implementation Changes

**File:** `src/mainwindow.cpp`

#### Added Includes
```cpp
#include <QDebug>
#include <DockManager.h>
#include <QLabel>
#include <QTextEdit>
#include <QMenuBar>
#include <QSettings>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include "mainwindow.h"
#include "appdockmanager.h"
#include "logger.h"
#include "canmanager.h"              // NEW
#include "cansimulationinterface.h"  // NEW
#include "canvectorxlinterface.h"    // NEW
```

#### Updated Constructor
```cpp
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Initialize logger FIRST
    Logger::instance().setLogLevel(LogLevel::Debug);
    Logger::instance().setLogToFile(true);
    Logger::instance().setLogToConsole(true);

    LOG_INFO(LogCategory::System, "Application starting...");

    // Setup window
    QString version = "1.0.0";
    setWindowTitle("SPYDER AutoTraceTool " + version);

    QIcon winIcon(":/ICONS/Resource/icons/app_icon.svg");
    winIcon.addFile(":/ICONS/Resource/icons/app_icon_square.svg");
    setWindowIcon(winIcon);

    // Initialize CAN system early  // NEW
    initializeCANSystem();

    // Initialize dock system
    initializeDockSystem();
    createMenus();

    // Load layout or show default
    if (m_appDockManager->hasSavedLayout()) {
        showDockLayout();
        m_appDockManager->restoreLayout();
        LOG_INFO(LogCategory::System, "Restored saved layout");
    } else {
        showWelcomePage();
        LOG_INFO(LogCategory::System, "Showing welcome page - no saved layout");
    }
}
```

#### New Method: initializeCANSystem()
```cpp
void MainWindow::initializeCANSystem()
{
    LOG_INFO(LogCategory::CAN, "Initializing CAN system...");

    CANManager& canMgr = CANManager::instance();

    // Register simulation interface
    auto* simInterface = new CANSimulationInterface(this);
    if (simInterface->initialize()) {
        canMgr.registerInterface("Simulation", simInterface);
        LOG_INFO(LogCategory::CAN, "CAN Simulation interface registered");
    } else {
        delete simInterface;
        LOG_WARNING(LogCategory::CAN, "Failed to initialize simulation interface");
    }

    // Register Vector XL interface (if available)
    auto* vectorInterface = new CANVectorXLInterface(this);
    if (vectorInterface->isAvailable()) {
        if (vectorInterface->initialize()) {
            canMgr.registerInterface("Vector XL", vectorInterface);
            LOG_INFO(LogCategory::CAN, "Vector XL interface registered");
        } else {
            delete vectorInterface;
            LOG_WARNING(LogCategory::CAN, "Failed to initialize Vector XL interface");
        }
    } else {
        delete vectorInterface;
        LOG_DEBUG(LogCategory::CAN, "Vector XL hardware not available");
    }

    // Set active interface to simulation
    canMgr.setActiveInterface("Simulation");

    // Connect CAN signals to main window for logging and UI updates
    connect(&canMgr, &CANManager::messageReceived,
            this, &MainWindow::onCANMessageReceived);

    connect(&canMgr, &CANManager::messageParsed,
            this, &MainWindow::onCANMessageParsed);

    connect(&canMgr, &CANManager::errorOccurred,
            this, &MainWindow::onCANError);

    // Try to load DBC database
    QString dbcPath = "Resource/dbc/MIB_MIBCAN.dbc";
    if (canMgr.loadDatabase(dbcPath)) {
        LOG_INFO(LogCategory::CAN, QString("DBC database loaded: %1").arg(dbcPath));
    } else {
        // Try alternative path
        dbcPath = "./Resource/dbc/MIB_MIBCAN.dbc";
        if (canMgr.loadDatabase(dbcPath)) {
            LOG_INFO(LogCategory::CAN, QString("DBC database loaded: %1").arg(dbcPath));
        } else {
            LOG_WARNING(LogCategory::CAN, "Failed to load DBC database - continuing without signal definitions");
        }
    }

    // Configure and open channel
    CANChannelConfig config;
    config.channel = 0;
    config.baudrate = 500000;
    config.fdEnabled = false;
    config.listenOnly = false;

    if (canMgr.openChannel(config)) {
        LOG_INFO(LogCategory::CAN, "CAN channel opened successfully");
        if (canMgr.setBusActive(true)) {
            LOG_INFO(LogCategory::CAN, "CAN bus activated");
        }
    } else {
        LOG_ERROR(LogCategory::CAN, "Failed to open CAN channel");
    }
}
```

#### New Signal Handlers
```cpp
void MainWindow::onCANMessageReceived(const CANMessage& msg)
{
    // Log received CAN message
    LOG_DEBUG(LogCategory::CAN, QString("CAN message received: %1").arg(msg.toString()));
}

void MainWindow::onCANMessageParsed(uint32_t id, const QMap<QString, QVariant>& signals)
{
    // Log parsed signal values
    QString signalStr;
    for (auto it = signals.begin(); it != signals.end(); ++it) {
        signalStr += QString("%1=%2 ").arg(it.key()).arg(it.value().toString());
    }
    LOG_DEBUG(LogCategory::CAN, 
              QString("CAN message parsed - ID: 0x%1 - %2").arg(id, 0, 16).arg(signalStr));
}

void MainWindow::onCANError(const QString& error)
{
    // Log CAN errors
    LOG_ERROR(LogCategory::CAN, QString("CAN error: %1").arg(error));
}
```

### 3. CANManager Header

**File:** `src/canmanager.h`

```cpp
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
```

### 4. CANManager Implementation (First Lines)

**File:** `src/canmanager.cpp`

```cpp
#include "canmanager.h"
#include "logger.h"

CANManager& CANManager::instance()
{
    static CANManager instance;
    return instance;
}

CANManager::CANManager()
    : m_activeInterface(nullptr)
{
}

CANManager::~CANManager()
{
    for (ICANInterface* interface : m_interfaces) {
        if (interface->isOpen()) {
            interface->close();
        }
        delete interface;
    }
}

// ... rest of implementation methods follow
```

## Usage Examples After Integration

### Example 1: Send a Raw CAN Message
```cpp
#include "canmanager.h"

void MyClass::sendEngineData()
{
    CANMessage msg;
    msg.id = 0x200;
    msg.dlc = 8;
    msg.data[0] = 100;  // Engine speed
    msg.data[1] = 50;   // Engine temp
    
    CANManager::instance().transmit(msg);
}
```

### Example 2: Send Message with Database Signals
```cpp
void MyClass::sendEngineRPM(double rpm)
{
    QMap<QString, QVariant> signals;
    signals["EngineSpeed"] = rpm;
    signals["EngineRunning"] = true;
    
    CANMessage msg = CANManager::instance().buildMessage(0x200, signals);
    CANManager::instance().transmit(msg);
}
```

### Example 3: Handle Received Messages
```cpp
// Already connected in initializeCANSystem()
void MainWindow::onCANMessageParsed(uint32_t id, const QMap<QString, QVariant>& signals)
{
    if (id == 0x200) {
        double rpm = signals.value("EngineSpeed", 0).toDouble();
        bool running = signals.value("EngineRunning", false).toBool();
        
        // Update UI, perform logic, etc.
        updateEngineGauge(rpm);
    }
}
```

### Example 4: Monitor Bus Statistics
```cpp
void MainWindow::updateStatistics()
{
    auto stats = CANManager::instance().getStatistics();
    
    qDebug() << "RX:" << stats.rxCount 
             << "TX:" << stats.txCount 
             << "Bus Load:" << stats.busLoad << "%";
}
```

## Summary of Changes

| Component | Change | Lines | Status |
|-----------|--------|-------|--------|
| canmanager.h | Fixed header | 60 | ✅ Complete |
| canmanager.cpp | Implementation only | 200 | ✅ Complete |
| mainwindow.h | Added slots | 6 | ✅ Complete |
| mainwindow.cpp | Init method + handlers | 120 | ✅ Complete |
| **Total** | **Full integration** | **~386** | ✅ **Complete** |

## Integration Checklist

- ✅ CANManager singleton implemented
- ✅ Interface abstraction in place
- ✅ Simulation interface available
- ✅ Vector XL interface available
- ✅ DBC database integration
- ✅ MainWindow initialization
- ✅ Signal/slot connections
- ✅ Error handling
- ✅ Logging integrated
- ✅ Dock widgets available
- ✅ Configuration settable

---

**See Also:**
- [CAN_INTEGRATION_SUMMARY.md](CAN_INTEGRATION_SUMMARY.md) - Overview
- [CAN_IMPLEMENTATION_GUIDE.md](CAN_IMPLEMENTATION_GUIDE.md) - Detailed guide
- [CAN_QUICK_REFERENCE.md](CAN_QUICK_REFERENCE.md) - Quick reference
