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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Initialize logger FIRST
    Logger::instance().setLogLevel(LogLevel::Debug);
    Logger::instance().setLogToFile(true);  // Auto-creates logs/spyder_log_TIMESTAMP.txt
    Logger::instance().setLogToConsole(true);

    LOG_INFO(LogCategory::System, "Application starting...");

    // Setup window
    QString version = "1.0.0";
    setWindowTitle("SPYDER AutoTraceTool " + version);

    QIcon winIcon(":/ICONS/Resource/icons/app_icon.svg");
    winIcon.addFile(":/ICONS/Resource/icons/app_icon_square.svg");
    setWindowIcon(winIcon);

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

MainWindow::~MainWindow()
{
    qDebug() << " Program ended";
}

void MainWindow::initializeDockSystem()
{
    // 1. Create central stacked widget
    m_centralStack = new QStackedWidget(this);
    setCentralWidget(m_centralStack);

    createWelcomePage();

    QWidget *dockContainer = new QWidget(this);
    m_appDockManager = new AppDockManager(dockContainer);

    // 5. Add widgets in correct order
    // Index 0: Welcome page
    // Index 1: Dock layout
    m_centralStack->addWidget(m_welcomePage);
    m_centralStack->addWidget(m_appDockManager->dockManager());

    // Create ALL docks using factory pattern
    m_appDockManager->createAllDocks();

    // OR create specific docks individually:
    // m_appDockManager->createDock(DockType::Log);
    // m_appDockManager->createDock(DockType::CANMessages);
    // m_appDockManager->createDock(DockType::TestSequencer);
    // ... etc

    connect(m_appDockManager, &AppDockManager::firstDockOpened,
            this, &MainWindow::onFirstDockOpened);
}

void MainWindow::createMenus()
{
    createFileMenu();
    createViewMenu();
    createToolsMenu();
    createHelpMenu();
}

void MainWindow::createFileMenu()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    QAction* newProjectAction = fileMenu->addAction(tr("New Project..."));
    connect(newProjectAction, &QAction::triggered, this, [this]() {
        LOG_INFO(LogCategory::UI, "New Project clicked");
        // Your implementation
    });

    QAction* openProjectAction = fileMenu->addAction(tr("Open Project..."));
    fileMenu->addSeparator();

    QAction* saveLayoutAction = fileMenu->addAction(tr("Save Layout"));
    connect(saveLayoutAction, &QAction::triggered, this, [this]() {
        m_appDockManager->saveLayout();
        LOG_INFO(LogCategory::UI, "Layout saved manually");
    });

    QAction* resetLayoutAction = fileMenu->addAction(tr("Reset to Default Layout"));
    connect(resetLayoutAction, &QAction::triggered, this, [this]() {
        m_appDockManager->loadDefaultLayout();
        LOG_INFO(LogCategory::UI, "Layout reset to default");
    });

    fileMenu->addSeparator();

    QAction* exitAction = fileMenu->addAction(tr("Exit"));
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
}

void MainWindow::createViewMenu()
{
    m_viewMenu = menuBar()->addMenu(tr("&View"));

    // Get grouped actions for better organization
    QMap<QString, QList<QAction*>> groups = m_appDockManager->getGroupedMenuActions();

    // Add actions grouped by category
    QStringList groupOrder = {"Core", "CAN Interface", "Serial Ports",
                              "Instruments", "Test Execution", "Diagnostics", "Additional"};

    for (const QString& groupName : groupOrder) {
        if (!groups.contains(groupName)) continue;

        QMenu* groupMenu = m_viewMenu->addMenu(groupName);
        for (QAction* action : groups[groupName]) {
            groupMenu->addAction(action);
        }
    }

    m_viewMenu->addSeparator();

    QAction* showAllAction = m_viewMenu->addAction(tr("Show All Panels"));
    connect(showAllAction, &QAction::triggered, this, [this]() {
        // Show all docks
        for (int i = 0; i < static_cast<int>(DockType::SystemMonitor) + 1; ++i) {
            m_appDockManager->showDock(static_cast<DockType>(i));
        }
        LOG_INFO(LogCategory::UI, "All panels shown");
    });

    QAction* hideAllAction = m_viewMenu->addAction(tr("Hide All Panels"));
    connect(hideAllAction, &QAction::triggered, this, [this]() {
        // Hide all except Log and essential
        for (int i = 0; i < static_cast<int>(DockType::SystemMonitor) + 1; ++i) {
            DockType type = static_cast<DockType>(i);
            if (type != DockType::Log && type != DockType::TestSequencer) {
                m_appDockManager->hideDock(type);
            }
        }
        LOG_INFO(LogCategory::UI, "All non-essential panels hidden");
    });
}

void MainWindow::createToolsMenu()
{
    QMenu* toolsMenu = menuBar()->addMenu(tr("&Tools"));

    QAction* canConfigAction = toolsMenu->addAction(tr("CAN Configuration..."));
    connect(canConfigAction, &QAction::triggered, this, [this]() {
        m_appDockManager->showDock(DockType::CANConfiguration);
        LOG_INFO(LogCategory::CAN, "CAN Configuration opened");
    });

    QAction* serialConfigAction = toolsMenu->addAction(tr("Serial Configuration..."));

    toolsMenu->addSeparator();

    QAction* settingsAction = toolsMenu->addAction(tr("Settings..."));
    connect(settingsAction, &QAction::triggered, this, [this]() {
        LOG_INFO(LogCategory::UI, "Settings dialog opened");
        // Show settings dialog
    });
}

void MainWindow::createHelpMenu()
{
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction* aboutAction = helpMenu->addAction(tr("About SPYDER"));
    connect(aboutAction, &QAction::triggered, this, [this]() {
        LOG_INFO(LogCategory::UI, "About dialog opened");
        QMessageBox::about(this, "About SPYDER AutoTraceTool",
                           "SPYDER AutoTraceTool v1.0.0\n\n"
                           "Professional Test and Measurement Tool\n"
                           "for Automotive Development");
    });
}

void MainWindow::createWelcomePage()
{
    m_welcomePage = new QWidget(this);

    auto *layout = new QVBoxLayout(m_welcomePage);
    layout->setAlignment(Qt::AlignCenter);

    auto *title = new QLabel("Welcome to SPYDER AutoTraceTool");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel("Serial & CAN Testing and Monitoring Tool");
    subtitle->setStyleSheet("font-size: 14px; color: gray;");
    subtitle->setAlignment(Qt::AlignCenter);

    auto *hint = new QLabel("Use <b>View</b> menu to open panels");
    hint->setStyleSheet("font-size: 12px; margin-top: 20px;");
    hint->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(10);
    layout->addWidget(subtitle);
    layout->addSpacing(30);
    layout->addWidget(hint);
    layout->addStretch();
}

void MainWindow::showWelcomePage()
{
    if (m_centralStack && m_welcomePage)
    {
        m_centralStack->setCurrentWidget(m_welcomePage);
        qDebug() << "Showing welcome page";
    }
}

void MainWindow::showDockLayout()
{
    if (m_centralStack && m_appDockManager)
    {
        m_centralStack->setCurrentWidget(m_appDockManager->dockManager());
        qDebug() << "Showing dock layout";
    }
}

void MainWindow::onFirstDockOpened()
{
    // Only switch to dock layout if we're currently on welcome page
    if (m_centralStack && m_centralStack->currentWidget() == m_welcomePage)
    {
        showDockLayout();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_appDockManager)
    {
        m_appDockManager->saveLayout();
    }
    QMainWindow::closeEvent(event);
}

/*
 *
 * // Example: Using the logger in your custom widgets
void SomeCustomWidget::performCANOperation()
{
    LOG_INFO(LogCategory::CAN, "Starting CAN operation");

    try {
        // Your CAN code here
        bool success = sendCANMessage();

        if (success) {
            LOG_INFO(LogCategory::CAN, "CAN message sent successfully");
        } else {
            LOG_ERROR(LogCategory::CAN, "Failed to send CAN message");
        }
    }
    catch (const std::exception& e) {
        LOG_CRITICAL(LogCategory::CAN,
                    QString("CAN operation failed: %1").arg(e.what()));
    }
}

// Example: Test execution with logging
void TestExecutor::runTest(const QString& testName)
{
    LOG_INFO(LogCategory::TestExecution,
            QString("Starting test: %1").arg(testName));

    // Test steps with detailed logging
    LOG_DEBUG(LogCategory::TestExecution, "Step 1: Initializing hardware");
    initHardware();

    LOG_DEBUG(LogCategory::TestExecution, "Step 2: Configuring CAN");
    configureCAN();

    LOG_DEBUG(LogCategory::TestExecution, "Step 3: Running test sequence");
    bool result = executeTestSequence();

    if (result) {
        LOG_INFO(LogCategory::TestExecution,
                QString("Test PASSED: %1").arg(testName));
    } else {
        LOG_ERROR(LogCategory::TestExecution,
                 QString("Test FAILED: %1").arg(testName));
    }
}

// Example: Serial port with categorized logging
void SerialPortHandler::openPort(int portNumber)
{
    LOG_INFO(LogCategory::Serial,
            QString("Opening serial port %1").arg(portNumber));

    if (!port.open()) {
        LOG_ERROR(LogCategory::Serial,
                 QString("Failed to open port %1: %2")
                 .arg(portNumber).arg(port.errorString()));
        return;
    }

    LOG_DEBUG(LogCategory::Serial,
             QString("Port %1 configured: %2 baud, 8N1")
             .arg(portNumber).arg(baudRate));
}
*/
