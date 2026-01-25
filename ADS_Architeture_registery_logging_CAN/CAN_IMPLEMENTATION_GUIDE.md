# CAN Integration - Implementation Guide

## Architecture Overview

### Layered Design

```
┌─────────────────────────────────────┐
│      MainWindow (UI Layer)          │
│  - Initializes CAN system           │
│  - Connects signals for logging     │
│  - Manages dock widgets             │
└────────────┬────────────────────────┘
             │ creates
             ▼
┌─────────────────────────────────────┐
│    CANManager (Singleton)           │
│  - Interface management             │
│  - Database management              │
│  - Message building/parsing         │
│  - Signal relay                     │
└────────────┬────────────────────────┘
             │ uses
     ┌───────┴────────┬─────────────┐
     ▼                ▼             ▼
┌─────────┐  ┌──────────────┐  ┌──────────┐
│Simulation│  │ Vector XL    │  │ DBC Db   │
│Interface │  │ Interface    │  │          │
└──────────┘  └──────────────┘  └──────────┘
```

## Core Components

### 1. CANManager (Singleton Pattern)
**Location:** [src/canmanager.h](src/canmanager.h), [src/canmanager.cpp](src/canmanager.cpp)

**Responsibilities:**
- Manage multiple CAN interface implementations
- Switch between active interfaces
- Load and manage DBC databases
- Provide high-level message API
- Relay interface signals with additional processing

**Key Methods:**
```cpp
// Interface management
bool registerInterface(const QString& name, ICANInterface* interface);
bool setActiveInterface(const QString& name);
ICANInterface* activeInterface() const;
QStringList availableInterfaces() const;

// Database operations
bool loadDatabase(const QString& filepath);
DBCDatabase* database();

// Message operations
bool transmit(const CANMessage& msg);
CANMessage buildMessage(uint32_t id, const QMap<QString, QVariant>& signalValues);
QMap<QString, QVariant> parseMessage(const CANMessage& msg);

// Status queries
bool isConnected() const;
bool isBusActive() const;
CANStatistics getStatistics() const;
```

### 2. ICANInterface (Abstract Base)
**Location:** [src/caninterface.h](src/caninterface.h)

**Derived Classes:**
- `CANSimulationInterface` - Software-based simulation
- `CANVectorXLInterface` - Vector XL hardware driver

**Interface Contract:**
```cpp
// Lifecycle
virtual bool initialize() = 0;
virtual bool open(const CANChannelConfig& config) = 0;
virtual bool close() = 0;

// Communication
virtual bool transmit(const CANMessage& msg) = 0;
virtual bool transmitBurst(const QVector<CANMessage>& messages) = 0;

// Signals emitted by all interfaces
void messageReceived(const CANMessage& msg);
void errorOccurred(const QString& error);
void busStateChanged(bool active);
void statisticsUpdated(const CANStatistics& stats);
```

### 3. MainWindow Integration
**Location:** [src/mainwindow.h](src/mainwindow.h), [src/mainwindow.cpp](src/mainwindow.cpp)

**Initialization Sequence:**
1. Logger setup
2. **`initializeCANSystem()`** ← New method
3. Dock system setup
4. Menus creation
5. Layout restoration

## Implementation Details

### CANManager Initialization

```cpp
void MainWindow::initializeCANSystem()
{
    CANManager& canMgr = CANManager::instance();
    
    // 1. Register interfaces
    auto* simInterface = new CANSimulationInterface(this);
    simInterface->initialize();
    canMgr.registerInterface("Simulation", simInterface);
    
    // 2. Load database
    canMgr.loadDatabase("Resource/dbc/MIB_MIBCAN.dbc");
    
    // 3. Configure channel
    CANChannelConfig config;
    config.baudrate = 500000;
    canMgr.openChannel(config);
    
    // 4. Activate bus
    canMgr.setBusActive(true);
    
    // 5. Connect signals
    connect(&canMgr, &CANManager::messageReceived,
            this, &MainWindow::onCANMessageReceived);
}
```

### Signal Flow Example

**When a CAN message arrives:**

```
CANVectorXLInterface
    │ (reads from hardware)
    ├─> CANMessage object created
    │
    └─> emit messageReceived(CANMessage)
        │
        └─> CANManager::onMessageReceived()
            │
            ├─> emit CANManager::messageReceived(msg)
            │   │
            │   └─> MainWindow::onCANMessageReceived(msg)
            │       └─> LOG_DEBUG(...) [Raw message log]
            │
            └─> [If database loaded]
                ├─> parseMessage(msg) → signals
                │
                └─> emit CANManager::messageParsed(id, signals)
                    │
                    └─> MainWindow::onCANMessageParsed(id, signals)
                        └─> LOG_DEBUG(...) [Decoded signals log]
```

## Configuration

### CANChannelConfig Structure
```cpp
struct CANChannelConfig {
    uint8_t channel;              // 0 or 1 (CAN HS/FD)
    uint32_t baudrate;            // e.g., 500000 for 500 kbps
    uint32_t dataBaudrate;        // For CAN FD (e.g., 2000000)
    bool fdEnabled;               // Enable CAN FD
    bool listenOnly;              // Listen-only mode (no transmission)
};
```

### Current Configuration (in initializeCANSystem)
```cpp
CANChannelConfig config;
config.channel = 0;
config.baudrate = 500000;         // 500 kbps (automotive standard)
config.fdEnabled = false;          // Standard CAN 2.0
config.listenOnly = false;         // Allow transmission
```

## Error Handling

### Graceful Degradation

**If DBC database unavailable:**
- Message objects can still be created manually
- Signal parsing unavailable but transmission works
- Raw CAN messages logged

**If Vector XL hardware unavailable:**
- Automatically falls back to Simulation interface
- User can switch interfaces manually
- Application continues normally

**If CAN channel fails to open:**
- Logged as ERROR
- Application continues with CAN inactive
- User can try again from configuration panel

### Logging Categories

```cpp
enum class LogCategory {
    System,
    UI,
    CAN,        // ← All CAN operations logged here
    Serial,
    DBC,
    Test
};
```

## Extending with New Interfaces

To add a new CAN interface (e.g., Peak PCAN):

```cpp
// 1. Create header (peakcaninterface.h)
class CANPeakInterface : public ICANInterface {
    CANInterfaceType type() const override { return CANInterfaceType::PeakCAN; }
    // ... implement interface
};

// 2. Register in initializeCANSystem()
auto* peakInterface = new CANPeakInterface(this);
if (peakInterface->isAvailable() && peakInterface->initialize()) {
    canMgr.registerInterface("Peak PCAN", peakInterface);
}

// 3. Make available in View menu automatically
// (AppDockManager already handles this)
```

## Message Building Examples

### Method 1: Raw Message
```cpp
CANMessage msg;
msg.id = 0x200;
msg.dlc = 8;
msg.data[0] = 0x12;
msg.data[1] = 0x34;
canMgr.transmit(msg);
```

### Method 2: Using Database Signals
```cpp
QMap<QString, QVariant> signals;
signals["EngineSpeed"] = 3000.0;
signals["EngineRunning"] = true;
signals["EngineTemp"] = 85.5;

CANMessage msg = canMgr.buildMessage(0x200, signals);
canMgr.transmit(msg);
```

### Method 3: Single Signal
```cpp
// Requires database loaded
canMgr.transmitSignal(0x200, "EngineSpeed", 3000.0);
```

## Statistics and Monitoring

```cpp
// Get real-time statistics
CANStatistics stats = canMgr.getStatistics();

qDebug() << "TX Messages:" << stats.txCount;
qDebug() << "RX Messages:" << stats.rxCount;
qDebug() << "Bus Load:" << stats.busLoad << "%";
qDebug() << "Errors:" << stats.errorCount;
```

## Testing Strategy

### Unit Testing
- Test message encoding/decoding with DBC
- Test interface switching logic
- Test error conditions

### Integration Testing
```cpp
// Test 1: Initialization
void testCANInitialization() {
    ASSERT_TRUE(CANManager::instance().isConnected());
    ASSERT_EQ(CANManager::instance().activeInterfaceName(), "Simulation");
}

// Test 2: Message transmission
void testMessageTransmit() {
    CANMessage msg{.id = 0x200, .dlc = 8};
    ASSERT_TRUE(CANManager::instance().transmit(msg));
}

// Test 3: Signal parsing
void testSignalParsing() {
    // Assuming database loaded
    CANMessage msg = canMgr.buildMessage(0x200, signals);
    auto parsed = canMgr.parseMessage(msg);
    ASSERT_EQ(parsed.size(), signals.size());
}
```

## Performance Considerations

### Memory Management
- `CANManager` is singleton - created once, never deleted
- Interfaces managed by CANManager (auto-deleted in destructor)
- Messages copied on signal emission (stack allocated)

### Thread Safety
- Current implementation assumes single-threaded Qt event loop
- Vector XL interface uses internal thread for receive
- Signal safety ensured through Qt's signal/slot mechanism

### CPU Usage
- Simulation interface: Minimal (no hardware)
- Vector XL interface: Depends on hardware and message rate
- Message parsing: O(n) where n = number of signals in message

## Debugging Tips

### Enable Debug Logging
```cpp
// In MainWindow constructor
Logger::instance().setLogLevel(LogLevel::Debug);
Logger::instance().setLogToConsole(true);
```

### Monitor CAN Bus
```cpp
// Add to MainWindow for real-time monitoring
QTimer* statsTimer = new QTimer(this);
connect(statsTimer, &QTimer::timeout, this, [this]() {
    auto stats = CANManager::instance().getStatistics();
    qDebug() << "Bus Load:" << stats.busLoad << "%";
});
statsTimer->start(1000); // Update every second
```

### Check Interface Status
```cpp
auto* iface = CANManager::instance().activeInterface();
if (iface) {
    qDebug() << "Interface:" << iface->name();
    qDebug() << "Available:" << iface->isAvailable();
    qDebug() << "Open:" << iface->isOpen();
    qDebug() << "Bus Active:" << iface->isBusActive();
    qDebug() << "Last Error:" << iface->getLastError();
}
```

## Common Issues and Solutions

### Issue: Messages not being received
- **Check:** Is bus active? `canMgr.isBusActive()`
- **Check:** Is interface open? `canMgr.isConnected()`
- **Check:** Correct CAN ID? Message IDs must match

### Issue: Signal parsing returns empty
- **Check:** Is DBC loaded? `canMgr.database()->isLoaded()`
- **Check:** Does message ID exist in DBC?
- **Check:** Do signal names match DBC exactly?

### Issue: Interface switching fails
- **Check:** Is interface registered? `canMgr.availableInterfaces()`
- **Check:** Close current interface before switching
- **Check:** New interface initialized?

