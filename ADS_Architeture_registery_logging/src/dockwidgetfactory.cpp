// dockwidgetfactory.cpp
#include "dockwidgetfactory.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>

ads::CDockWidget* DockWidgetFactory::createDock(const DockConfig& config)
{
    auto* dock = new ads::CDockWidget(config.title);
    dock->setObjectName(config.objectName);

    // Create widget content
    QWidget* widget = nullptr;
    if (config.widgetFactory) {
        widget = config.widgetFactory();
    } else {
        widget = createPlaceholderWidget(config.title);
    }

    dock->setWidget(widget);

    // Set dock features
    ads::CDockWidget::DockWidgetFeatures features =
        ads::CDockWidget::DockWidgetMovable |
        ads::CDockWidget::DockWidgetPinnable;

    if (config.allowFloating) {
        features |= ads::CDockWidget::DockWidgetFloatable;
    }

    if (config.allowClosing) {
        features |= ads::CDockWidget::DockWidgetClosable;
    }

    dock->setFeatures(features);

    // Apply custom styling
    applyDockStyle(dock);

    // Set icon if provided
    if (!config.iconPath.isEmpty()) {
        dock->setIcon(QIcon(config.iconPath));
    }

    return dock;
}

QWidget* DockWidgetFactory::createPlaceholderWidget(const QString& text)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QLabel* label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 14px; color: #888;");

    layout->addWidget(label);
    return widget;
}

void DockWidgetFactory::applyDockStyle(ads::CDockWidget* dock)
{
    // Custom styling for professional look
    // You can expand this based on your theme requirements
}

DockConfig DockWidgetFactory::getConfig(DockType type)
{
    switch (type) {
    // Core Views
    case DockType::ProjectExplorer: {
        DockConfig cfg(type, "dock.project_explorer", "Project Explorer");
        cfg.defaultArea = ads::LeftDockWidgetArea;
        return cfg;
    }

    case DockType::Properties: {
        DockConfig cfg(type, "dock.properties", "Properties");
        cfg.defaultArea = ads::RightDockWidgetArea;
        return cfg;
    }

    case DockType::Log: {
        DockConfig cfg(type, "dock.log", "System Log");
        cfg.defaultArea = ads::BottomDockWidgetArea;
        cfg.allowClosing = false; // Always keep log visible
        cfg.startVisible = true;
        return cfg;
    }

    // CAN Interface
    case DockType::CANMessages: {
        DockConfig cfg(type, "dock.can_messages", "CAN Messages");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }
    case DockType::CANConfiguration: {
        DockConfig cfg(type, "dock.can_config", "CAN Configuration");
        cfg.defaultArea = ads::RightDockWidgetArea;
        return cfg;
    }

    case DockType::CANDatabase: {
        DockConfig cfg(type, "dock.can_database", "CAN Database");
        cfg.defaultArea = ads::LeftDockWidgetArea;
        return cfg;
    }

    case DockType::CANSignalMonitor: {
        DockConfig cfg(type, "dock.can_signals", "Signal Monitor");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }

    // Serial Ports
    case DockType::SerialPort1: {
        DockConfig cfg(type, "dock.serial_port1", "Serial Port 1");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }

    case DockType::SerialPort2: {
        DockConfig cfg(type, "dock.serial_port2", "Serial Port 2");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }

    case DockType::SerialPort3: {
        DockConfig cfg(type, "dock.serial_port3", "Serial Port 3");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }

    case DockType::SerialPort4: {
        DockConfig cfg(type, "dock.serial_port4", "Serial Port 4");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }
    case DockType::SerialTerminal: {
        DockConfig cfg(type, "dock.serial_terminal", "Serial Terminal");
        cfg.defaultArea = ads::BottomDockWidgetArea;
        return cfg;
    }

    // Instruments
    case DockType::PowerSupply: {
        DockConfig cfg(type, "dock.power_supply", "Power Supply");
        cfg.defaultArea = ads::RightDockWidgetArea;
        return cfg;
    }
    case DockType::Oscilloscope: {
        DockConfig cfg(type, "dock.oscilloscope", "Oscilloscope");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }

    case DockType::DMM: {
        DockConfig cfg(type, "dock.dmm", "Digital Multimeter");
        cfg.defaultArea = ads::RightDockWidgetArea;
        return cfg;
    }

    case DockType::ModbusRelay: {
        DockConfig cfg(type, "dock.modbus_relay", "Modbus Relay");
        cfg.defaultArea = ads::RightDockWidgetArea;
        return cfg;
    }

    // Test Execution
    case DockType::TestSequencer: {
        DockConfig cfg(type, "dock.test_sequencer", "Test Sequencer");
        cfg.defaultArea = ads::LeftDockWidgetArea;
        cfg.allowClosing = false;
        cfg.startVisible = true;
        return cfg;
    }

    case DockType::TestResults: {
        DockConfig cfg(type, "dock.test_results", "Test Results");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }

    case DockType::TestVariables: {
        DockConfig cfg(type, "dock.test_variables", "Test Variables");
        cfg.defaultArea = ads::RightDockWidgetArea;
        return cfg;
    }

    case DockType::TestReport: {
        DockConfig cfg(type, "dock.test_report", "Test Report");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }

        // Diagnostics
    case DockType::DTCMonitor: {
        DockConfig cfg(type, "dock.dtc_monitor", "DTC Monitor");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }

    case DockType::DIDReader: {
        DockConfig cfg(type, "dock.did_reader", "DID Reader");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }

    case DockType::TraceMonitor: {
        DockConfig cfg(type, "dock.trace_monitor", "Trace Monitor");
        cfg.defaultArea = ads::BottomDockWidgetArea;
        return cfg;
    }

        // Additional
    case DockType::ScriptEditor: {
        DockConfig cfg(type, "dock.script_editor", "Script Editor");
        cfg.defaultArea = ads::CenterDockWidgetArea;
        return cfg;
    }

    case DockType::DataRecorder: {
        DockConfig cfg(type, "dock.data_recorder", "Data Recorder");
        cfg.defaultArea = ads::BottomDockWidgetArea;
        return cfg;
    }

    case DockType::SystemMonitor: {
        DockConfig cfg(type, "dock.system_monitor", "System Monitor");
        cfg.defaultArea = ads::RightDockWidgetArea;
        return cfg;
    }

    default:
        return DockConfig(type, "dock.unknown", "Unknown");
    }
}

QList<DockConfig> DockWidgetFactory::getAllConfigs()
{
    QList<DockConfig> configs;

    // Add all dock types in logical order
    configs << getConfig(DockType::ProjectExplorer)
            << getConfig(DockType::Properties)
            << getConfig(DockType::Log)

            // CAN Group
            << getConfig(DockType::CANMessages)
            << getConfig(DockType::CANConfiguration)
            << getConfig(DockType::CANDatabase)
            << getConfig(DockType::CANSignalMonitor)

            // Serial Group
            << getConfig(DockType::SerialPort1)
            << getConfig(DockType::SerialPort2)
            << getConfig(DockType::SerialPort3)
            << getConfig(DockType::SerialPort4)
            << getConfig(DockType::SerialTerminal)

            // Instruments Group
            << getConfig(DockType::PowerSupply)
            << getConfig(DockType::Oscilloscope)
            << getConfig(DockType::DMM)
            << getConfig(DockType::ModbusRelay)

            // Test Group
            << getConfig(DockType::TestSequencer)
            << getConfig(DockType::TestResults)
            << getConfig(DockType::TestVariables)
            << getConfig(DockType::TestReport)

            // Diagnostics Group
            << getConfig(DockType::DTCMonitor)
            << getConfig(DockType::DIDReader)
            << getConfig(DockType::TraceMonitor)

            // Additional
            << getConfig(DockType::ScriptEditor)
            << getConfig(DockType::DataRecorder)
            << getConfig(DockType::SystemMonitor);

    return configs;
}
