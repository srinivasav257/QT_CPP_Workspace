# QT_CPP_Workspace
practice designs and sample architeture ideas

🎯 Key Components
1. DockWidgetFactory (Standardized Widget Creation)

Enum for all 25+ dock types (CAN, Serial, Instruments, Test Execution, etc.)
Centralized configuration per dock type
Consistent styling and behavior
Easy to extend for new widgets

2. Centralized Logger (Production-Ready Debugging)

Log Levels: Trace, Debug, Info, Warning, Error, Critical, Fatal
15 Categories: System, CAN, Serial, PowerSupply, TestExecution, Diagnostics, etc.
Thread-safe with QMutex
Dual output: Console + File (with auto-rotation)
Filtering: By level, category, search text
Buffer management: Configurable max size (default 10K entries)

3. LogViewerWidget (Live Log UI)

Real-time log display with color coding
Filterable by level, category, search text
Auto-scroll option
Export to file
Professional formatting

4. Updated AppDockManager (Scalable Architecture)

Factory pattern for creating docks
Organized menu with grouped actions
Layout persistence
Easy programmatic control (show/hide/query docks)

✨ Benefits
✅ Scalability: Easy to add 50+ docks without code duplication
✅ Debugging: Track issues across CAN, Serial, Instruments with categorized logs
✅ Maintainability: Single place to configure each dock type
✅ Professional: Thread-safe, filterable, exportable logs
✅ User Experience: Grouped menus, persistent layouts, live filtering


🎯 Complete CAN System Components
1. Core Interface Architecture

ICANInterface - Abstract base class for all hardware
Supports CAN 2.0 and CAN FD
Statistics tracking (TX/RX counts, bus load, errors)
Unified API across all hardware types

2. Implemented Interfaces
Simulation Interface ✅

Software-only testing without hardware
Periodic message generation
Auto-response capability
Bus error simulation
Perfect for development/testing

Vector XL Interface ✅ (Framework ready)

Complete structure for Vector XL API integration
Just uncomment sections and link vxlapi.dll
Device enumeration
CAN FD support
Ready for production hardware

3. DBC Parser 🔥

Full DBC file parsing
Message and signal definitions
Value tables (enum values)
Comments and attributes
Automatic signal encoding/decoding
Supports Intel (Little Endian) and Motorola (Big Endian) byte order

4. CAN Trace Window (CANoe-style)

Real-time message display
Filters by Channel, ID, search text
Auto-scroll and pause
Decoded signal values from DBC
Export to CSV/TXT
Statistics (message count, rate)
Color coding for RTR frames

5. CANManager (Centralized Control)

Singleton pattern for application-wide access
Interface registration and switching
DBC database management
High-level API: transmit(), parseMessage(), buildMessage()
Signal-level operations: transmitSignal("EngineSpeed", 2500)

🔌 Extending for Other Hardware
To add Peak, Kvaser, etc., just:

Inherit from ICANInterface
Implement the virtual methods
Register with CANManager

🎯 CAN Configuration Dialog
Features:
✅ Multi-channel support (2 channels: HS & FD)
✅ Interface selection (Simulation, Vector XL, etc.)
✅ Device enumeration with refresh
✅ Baudrate configuration (presets + custom)
✅ CAN FD support with data baudrate
✅ Advanced timing parameters (SJW, TSEG1, TSEG2)
✅ DBC file browser per channel
✅ Test connection button
✅ Persistent settings (QSettings)
✅ Auto-connect on startup option
Usage:
cppm_canConfigDialog = new CANConfigDialog(this);
m_canConfigDialog->exec();

🎯 IG Block System (Interaction Layer)
Block Types Implemented:

CANTransmit - Send CAN messages (single or cyclic)
CANReceive - Wait for CAN message with validation
CANMonitor - Monitor signal values over time (avg/min/max)
Delay - Time delay
WaitCondition - Wait for custom condition
Variable - Set/modify test variables

IGSequence Features:

Visual drag-drop editor
Sequential execution
Real-time status updates
Save/load sequences (.igs files)
Block palette with all types
Move up/down, add/remove blocks
Execute/Stop/Pause controls

Example Test Sequence:

IGSequence* seq = new IGSequence("ECU Power-On Test");

// 1. Wait for ECU alive message
auto* rx = new IGBlockCANReceive("Wait ECU Alive");
rx->setMessageID(0x100);
rx->setTimeout(3000);
seq->addBlock(rx);

// 2. Verify status signal
auto* verify = new IGBlockCANReceive("Verify Status");
verify->setMessageID(0x100);
verify->setExpectedSignal("ECU_Status");
verify->setExpectedValue(1);
seq->addBlock(verify);

// 3. Monitor engine RPM
auto* monitor = new IGBlockCANMonitor("Monitor RPM");
monitor->setMessageID(0x200);
monitor->setSignalName("EngineSpeed");
monitor->setMonitorDuration(2000);
seq->addBlock(monitor);

// Execute!
seq->execute();
```

---

## 📊 **Complete System Architecture**
```
┌────────────────────────────────────────────────┐
│           MainWindow (SPYDER Tool)             │
├────────────────────────────────────────────────┤
│                                                │
│  ┌─────────────┐  ┌──────────────┐  ┌────────┐ │
│  │ CAN Config  │  │ IG Sequencer │  │ Trace  │ │
│  │   Dialog    │  │    Editor    │  │ Window │ │
│  └──────┬──────┘  └──────┬───────┘  └───┬────┘ │
│         │                │              │      │
│         └────────┬───────┴──────────────┘      │
│                  │                             │
│         ┌────────▼────────┐                    │
│         │  CANManager     │◄───────────────┐   │
│         └────────┬────────┘                │   │
│                  │                         │   │
│    ┌─────────────┴──────────────┐          │   │
│    │                            │          │   │
│ ┌──▼────────┐        ┌──────────▼──┐   ┌───┴──┐│
│ │Simulation │        │  Vector XL  │   │  DBC ││
│ │ Interface │        │  Interface  │   │Parser││
│ └───────────┘        └─────────────┘   └──────┘│
└────────────────────────────────────────────────┘

🚀 Key Integration Points
1. Initialize Everything:
void MainWindow::initializeCANSystem() {
    // Register interfaces
    CANManager::instance().registerInterface("Simulation", new CANSimulationInterface());
    
    // Setup config dialog
    m_canConfigDialog = new CANConfigDialog(this);
    
    // Setup IG sequencer
    m_igSequenceEditor = new IGSequenceEditor(this);
    
    // Add to docks
    createDockForWidget(DockType::TestSequencer, m_igSequenceEditor);
}

2. Connect Config to CAN Manager:
cppconnect(m_canConfigDialog, &CANConfigDialog::settingsApplied,
        this, &MainWindow::applyCANSettings);
        
3. Build Test Sequences:
Use the visual editor OR programmatically create sequences for automated testing.

💡 Next Steps
You now have:

✅ Complete CAN communication stack
✅ DBC parsing and signal handling
✅ Configuration management
✅ Visual test sequencer
✅ Block-based test automation

Ready to add:

Serial port blocks (similar pattern to CAN blocks)
Power supply blocks
DMM measurement blocks
Relay control blocks
Script execution blocks
