# CAN Integration - Visual Architecture

## System Block Diagram

```
┌────────────────────────────────────────────────────────────────────┐
│                           SPYDER Application                       │
│                                                                    │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │                      MainWindow                              │ │
│  │                                                              │ │
│  │  Constructor:                                               │ │
│  │  1. Initialize Logger                                       │ │
│  │  2. initializeCANSystem() ←────────────┐                    │ │
│  │  3. initializeDockSystem()             │                    │ │
│  │  4. Create Menus                       │                    │ │
│  │  5. Restore Layout                     │                    │ │
│  └──────────────────────────────────────┬─┘                    │ │
│                                         │                      │ │
│  Signal Handlers:                       │                      │ │
│  • onCANMessageReceived()               │                      │ │
│  • onCANMessageParsed()                 │                      │ │
│  • onCANError()                         │                      │ │
└──────────────────────────────────────────┼─────────────────────┘ │
                                           │                        │
                                           ▼                        │
              ┌────────────────────────────────────────────┐       │
              │       CANManager (Singleton)              │       │
              │                                            │       │
              │  ┌──────────────────────────────────┐    │       │
              │  │ Interface Management             │    │       │
              │  │ - Register interfaces            │    │       │
              │  │ - Switch active interface        │    │       │
              │  │ - Get interface by name          │    │       │
              │  └──────────────────────────────────┘    │       │
              │                                            │       │
              │  ┌──────────────────────────────────┐    │       │
              │  │ Message Management               │    │       │
              │  │ - Transmit messages              │    │       │
              │  │ - Build messages from signals    │    │       │
              │  │ - Parse messages to signals      │    │       │
              │  └──────────────────────────────────┘    │       │
              │                                            │       │
              │  ┌──────────────────────────────────┐    │       │
              │  │ Database Management              │    │       │
              │  │ - Load DBC file                  │    │       │
              │  │ - Manage symbol table            │    │       │
              │  │ - Signal encoding/decoding       │    │       │
              │  └──────────────────────────────────┘    │       │
              │                                            │       │
              │  Signals Emitted:                        │       │
              │  • messageReceived()                     │       │
              │  • messageParsed()                       │       │
              │  • errorOccurred()                       │       │
              │  • interfaceChanged()                    │       │
              │  • databaseLoaded()                      │       │
              └────────┬───────────────────┬──────────────┘       │
                       │                   │                      │
                       ▼                   ▼                      │
        ┌──────────────────────┐  ┌──────────────────────┐       │
        │ ICANInterface        │  │  DBCDatabase         │       │
        │ (Abstract Base)      │  │                      │       │
        │                      │  │  - Messages          │       │
        │ + type()             │  │  - Signals           │       │
        │ + name()             │  │  - Encoding tables   │       │
        │ + initialize()       │  │                      │       │
        │ + open()             │  │  getSignal()         │       │
        │ + close()            │  │  encodeData()        │       │
        │ + transmit()         │  │  parseData()         │       │
        │                      │  │                      │       │
        │ Signals:             │  └──────────────────────┘       │
        │ + messageReceived()  │                                  │
        │ + errorOccurred()    │                                  │
        │ + busStateChanged()  │                                  │
        └──────┬─────────────────┘                                │
               │                                                  │
        ┌──────┴──────────────┬──────────────────────┐           │
        │                     │                      │           │
        ▼                     ▼                      ▼           │
   ┌─────────────┐   ┌─────────────────┐   ┌──────────────┐    │
   │ Simulation  │   │  Vector XL      │   │ Future: Peak │    │
   │ Interface   │   │  Interface      │   │ PCAN, etc.   │    │
   │             │   │                 │   │              │    │
   │ • Software  │   │ • Hardware      │   │ • Custom     │    │
   │ • Fast      │   │ • Real CAN bus  │   │ • User       │    │
   │ • Testing   │   │ • Production    │   │   defined    │    │
   └─────────────┘   └─────────────────┘   └──────────────┘    │
                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## Message Flow Diagram

### Receiving a CAN Message

```
CAN Bus / Simulation
    │
    ▼
┌────────────────────────────┐
│  CAN Interface             │
│  (reads hardware/simulates)│
└────────────────┬───────────┘
                 │
                 ├─► Parse bytes into CANMessage struct
                 │
                 ▼
            emit messageReceived(CANMessage msg)
                 │
                 ▼
┌────────────────────────────────────────────┐
│  CANManager::onMessageReceived()           │
│                                             │
│  1. Emit messageReceived() signal           │
│     └─► MainWindow::onCANMessageReceived() │
│         └─► LOG_DEBUG(raw message)         │
│                                             │
│  2. If DBC loaded:                         │
│     a. Parse message → signals             │
│     b. Emit messageParsed() signal         │
│        └─► MainWindow::onCANMessageParsed()│
│            └─► LOG_DEBUG(decoded signals) │
└────────────────────────────────────────────┘
                 │
                 ▼
        Logged to console & file
```

### Sending a CAN Message

```
Application Code
    │
    ├─► CANManager::transmit(msg)
    │
    ├─► or CANManager::buildMessage(id, signals)
    │
    ├─► or CANManager::transmitSignal(msgId, signal, value)
    │
    ▼
┌────────────────────────────────────┐
│  CANManager                        │
│                                    │
│  Validate & forward to interface   │
└────────────┬──────────────────────┘
             │
             ▼
┌────────────────────────────────────┐
│  ICANInterface (active)            │
│                                    │
│  - Simulation: Inject into buffer  │
│  - Vector XL: Send via hardware    │
│  - Others: Custom implementation   │
└────────────┬──────────────────────┘
             │
             ▼
        CAN Bus / Network
```

## Data Structure Diagram

```
CANMessage
├─ uint32_t id          (0x000-0x7FF for standard, 0x00000000-0x1FFFFFFF for extended)
├─ uint8_t data[8]      (Raw bytes)
├─ uint8_t dlc          (Data Length Code, 0-8)
├─ bool isExtended      (Standard vs Extended ID)
├─ bool isRTR           (Remote Transmission Request)
├─ bool isFD            (CAN FD flag)
├─ uint64_t timestamp   (Microseconds)
└─ uint8_t channel      (0 or 1)

CANChannelConfig
├─ uint8_t channel       (CAN channel: 0=HS, 1=FD)
├─ uint32_t baudrate     (Bits per second, e.g., 500000)
├─ uint32_t dataBaudrate (For CAN FD data phase)
├─ bool fdEnabled        (Enable CAN FD)
└─ bool listenOnly       (Read-only mode)

CANStatistics
├─ uint64_t txCount      (Messages transmitted)
├─ uint64_t rxCount      (Messages received)
├─ uint64_t errorCount   (Error frames)
├─ uint32_t busLoad      (0-100%)
├─ uint32_t txErrors     (TX error counter)
└─ uint32_t rxErrors     (RX error counter)

DBCMessage
├─ uint32_t id
├─ QString name
├─ uint8_t dlc
├─ QVector<DBCSignal> signals
├─ encodeData(uint8_t* data, QMap<QString, QVariant>& signals)
└─ parseData(uint8_t* data, uint8_t dlc) → QMap<QString, QVariant>

DBCSignal
├─ QString name
├─ uint32_t startBit
├─ uint32_t length
├─ double scale
├─ double offset
├─ double min
├─ double max
└─ QString unit
```

## Initialization Sequence Diagram

```
Application Start
        │
        ▼
┌──────────────────┐
│ main()           │
└────────┬─────────┘
         │
         ▼
┌──────────────────────────────────────┐
│ MainWindow Constructor               │
└────────┬─────────────────────────────┘
         │
         ├─► Logger::setLogLevel()
         │
         ├─► Logger::setLogToFile()
         │
         ├─► Logger::setLogToConsole()
         │
         ├─► setWindowTitle()
         │
         ├─► setWindowIcon()
         │
         ├─► initializeCANSystem()  ◄────────────────────┐
         │   ├─► CANManager::instance()                  │
         │   │                                            │
         │   ├─► new CANSimulationInterface()             │
         │   │   └─► simInterface->initialize()          │
         │   │   └─► registerInterface("Simulation")     │
         │   │                                            │
         │   ├─► new CANVectorXLInterface()               │
         │   │   └─► isAvailable()?                      │
         │   │       ├─► YES: initialize() & register   │
         │   │       └─► NO: delete               New CAN│
         │   │                                     System│
         │   ├─► setActiveInterface("Simulation")        │
         │   │                                            │
         │   ├─► connect(messageReceived signal)         │
         │   │                                            │
         │   ├─► loadDatabase("Resource/dbc/...")        │
         │   │   ├─► Success → LOG_INFO                  │
         │   │   └─► Fail → Try alt path                 │
         │   │                                            │
         │   ├─► CANChannelConfig config                 │
         │   │   └─► baudrate = 500000                   │
         │   │                                            │
         │   ├─► openChannel(config)                     │
         │   │   └─► Success → LOG_INFO                  │
         │   │   └─► Fail → LOG_ERROR                    │
         │   │                                            │
         │   └─► setBusActive(true)                      │
         │       └─► LOG_INFO                            │
         │                                                │
         ├─► initializeDockSystem()                      │
         │   ├─► Create stacked widget                  │
         │   ├─► Create AppDockManager                  │
         │   ├─► createAllDocks()                       │
         │   └─► Connect firstDockOpened signal         │
         │                                                │
         ├─► createMenus()                               │
         │   ├─► createFileMenu()                        │
         │   ├─► createViewMenu()                        │
         │   ├─► createToolsMenu()                       │
         │   └─► createHelpMenu()                        │
         │                                                │
         ├─► hasSavedLayout()?                           │
         │   ├─► YES: showDockLayout()                   │
         │   │        restoreLayout()                    │
         │   └─► NO: showWelcomePage()                   │
         │                                                │
         └─► MainWindow visible                         │
                                                          │
         Application Ready ◄──────────────────────────────┘
```

## Signal Connection Diagram

```
┌──────────────────────────────┐
│    CAN Hardware / Sim        │
└────────┬─────────────────────┘
         │
         ▼
┌────────────────────────────────────────────┐
│  ICANInterface::messageReceived()           │
└────────┬───────────────────────────────────┘
         │ (Qt Signal)
         ├─────────────┐
         │             │
         ▼             ▼
    ┌─────────────────────────────────┐
    │ CANManager::onMessageReceived()  │ ◄─────┐
    │                                  │       │
    │ 1. emit messageReceived(msg)     │       │
    │ 2. emit messageParsed(...)       │       │ Connected
    └─────┬──────────┬─────────────────┘       │ in
          │          │                    initializeCANSystem()
          │          │
          ▼          ▼
    ┌─────────┐  ┌──────────────┐
    │ Signal  │  │ Signal       │
    │ 1       │  │ 2            │
    └────┬────┘  └────┬─────────┘
         │             │
         ▼             ▼
    ┌────────────────────────────────────────────┐
    │ MainWindow::onCANMessageReceived()          │
    │ MainWindow::onCANMessageParsed()            │
    │                                             │
    │ ├─► LOG_DEBUG(...)                         │
    │ └─► Update UI panels                       │
    └────────────────────────────────────────────┘
```

## State Machine: Interface Lifecycle

```
              ┌──────────────┐
              │  Not Created │
              └───────┬──────┘
                      │ new Interface()
                      ▼
              ┌──────────────┐
              │ Constructed  │ ◄───────┐
              └───────┬──────┘         │
                      │                 │ close()
                      │ initialize()     │
                      ▼                 │
              ┌──────────────┐          │
              │ Initialized  │          │
              └───────┬──────┘          │
                      │                 │
                      │ open(config)    │
                      ▼                 │
              ┌──────────────┐          │
         ┌───►│ Connected    │──────────┘
         │    └──────┬───────┘
         │           │
         │ error     │ setBusActive(true)
         │           ▼
         │    ┌──────────────┐
         │    │ Bus Active   │ ◄─────┐
         │    └───────┬──────┘       │
         │            │ setBusActive(false)
         └────────────┤
                      │
                      ▼
                  Error State
                      │
                      ▼
              Close & Cleanup
```

## File Organization

```
ADS_Architecture_registery_logging_CAN/
│
├── src/
│   ├── caninterface.h           (Abstract base)
│   ├── canmanager.h             (Singleton manager) ✅ FIXED
│   ├── canmanager.cpp           (Implementation) ✅ FIXED
│   ├── cansimulationinterface.h/cpp
│   ├── canvectorxlinterface.h/cpp
│   ├── cantracewindow.h/cpp
│   ├── dbcparser.h/cpp
│   ├── mainwindow.h             (Updated) ✅
│   ├── mainwindow.cpp           (Updated) ✅
│   ├── appdockmanager.h/cpp
│   ├── dockwidgetfactory.h/cpp
│   ├── logger.h/cpp
│   └── ...
│
├── Resource/
│   ├── dbc/
│   │   ├── MIB_MIBCAN.dbc
│   │   └── Modified_MLB...dbc
│   └── icons/
│
├── third_party/
│   └── qtads/
│
├── CMakeLists.txt
├── README.md
│
└── Documentation/
    ├── CAN_INTEGRATION_SUMMARY.md ✅
    ├── CAN_IMPLEMENTATION_GUIDE.md ✅
    ├── CAN_QUICK_REFERENCE.md ✅
    ├── CAN_CODE_REFERENCE.md ✅
    └── CAN_ARCHITECTURE.md (this file) ✅
```

---

**Integration Status: ✅ COMPLETE**

All CAN functionality is fully integrated into the main SPYDER AutoTraceTool application.
