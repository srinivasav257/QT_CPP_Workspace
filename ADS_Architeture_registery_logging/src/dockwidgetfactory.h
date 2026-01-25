// dockwidgetfactory.h
#pragma once

#include <QWidget>
#include <QString>
#include <DockWidget.h>

namespace ads { class CDockWidget; }

// Enum for all dock types in your application
enum class DockType {
    // Core Views
    ProjectExplorer,
    Properties,
    Log,

    // CAN Interface
    CANMessages,
    CANConfiguration,
    CANDatabase,
    CANSignalMonitor,

    // Serial Interface
    SerialPort1,
    SerialPort2,
    SerialPort3,
    SerialPort4,
    SerialTerminal,

    // Instruments
    PowerSupply,
    Oscilloscope,
    DMM,
    ModbusRelay,

    // Test Execution
    TestSequencer,
    TestResults,
    TestVariables,
    TestReport,

    // Diagnostics
    DTCMonitor,
    DIDReader,
    TraceMonitor,

    // Additional
    ScriptEditor,
    DataRecorder,
    SystemMonitor
};

// Configuration for each dock widget
struct DockConfig {
    DockType type;
    QString objectName;
    QString title;
    QString iconPath;
    ads::DockWidgetArea defaultArea;
    bool startVisible;
    bool allowFloating;
    bool allowClosing;

    // Factory method to create the widget content
    std::function<QWidget*()> widgetFactory;

    DockConfig(DockType t, const QString& name, const QString& title)
        : type(t)
        , objectName(name)
        , title(title)
        , defaultArea(ads::CenterDockWidgetArea)
        , startVisible(false)
        , allowFloating(true)
        , allowClosing(true)
        , widgetFactory(nullptr)
    {}
};

class DockWidgetFactory
{
public:
    // Create a standardized dock widget
    static ads::CDockWidget* createDock(const DockConfig& config);

    // Get predefined configurations for all dock types
    static DockConfig getConfig(DockType type);

    // Get all available dock configurations
    static QList<DockConfig> getAllConfigs();

private:
    static QWidget* createPlaceholderWidget(const QString& text);
    static void applyDockStyle(ads::CDockWidget* dock);
};
