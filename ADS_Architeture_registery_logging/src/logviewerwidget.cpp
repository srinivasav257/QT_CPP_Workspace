// logviewerwidget.cpp
#include "logviewerwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QScrollBar>

LogViewerWidget::LogViewerWidget(QWidget* parent)
    : QWidget(parent)
    , m_minLevelFilter(LogLevel::Trace)
    , m_categoryFilter(static_cast<LogCategory>(-1))  // All categories
    , m_autoScroll(true)
{
    setupUI();

    // Connect to logger
    connect(&Logger::instance(), &Logger::logEntryAdded,
            this, &LogViewerWidget::onLogEntryAdded);

    connect(&Logger::instance(), &Logger::logsCleared,
            this, [this]() { m_logDisplay->clear(); });

    // Load existing logs
    updateDisplay();
}

void LogViewerWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Toolbar
    QHBoxLayout* toolbarLayout = new QHBoxLayout();

    // Level filter
    toolbarLayout->addWidget(new QLabel("Level:"));
    m_levelFilter = new QComboBox();
    m_levelFilter->addItem("All", -1);
    m_levelFilter->addItem("TRACE", static_cast<int>(LogLevel::Trace));
    m_levelFilter->addItem("DEBUG", static_cast<int>(LogLevel::Debug));
    m_levelFilter->addItem("INFO", static_cast<int>(LogLevel::Info));
    m_levelFilter->addItem("WARNING", static_cast<int>(LogLevel::Warning));
    m_levelFilter->addItem("ERROR", static_cast<int>(LogLevel::Error));
    m_levelFilter->addItem("CRITICAL", static_cast<int>(LogLevel::Critical));
    m_levelFilter->setCurrentIndex(0);
    connect(m_levelFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogViewerWidget::onLevelFilterChanged);
    toolbarLayout->addWidget(m_levelFilter);

    // Category filter
    toolbarLayout->addWidget(new QLabel("Category:"));
    m_categoryCombo = new QComboBox();
    m_categoryCombo->addItem("All", -1);
    m_categoryCombo->addItem("System", static_cast<int>(LogCategory::System));
    m_categoryCombo->addItem("CAN", static_cast<int>(LogCategory::CAN));
    m_categoryCombo->addItem("Serial", static_cast<int>(LogCategory::Serial));
    m_categoryCombo->addItem("Test Exec", static_cast<int>(LogCategory::TestExecution));
    m_categoryCombo->addItem("Diagnostics", static_cast<int>(LogCategory::Diagnostics));
    // Add more as needed
    connect(m_categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogViewerWidget::onCategoryFilterChanged);
    toolbarLayout->addWidget(m_categoryCombo);

    // Search box
    toolbarLayout->addWidget(new QLabel("Search:"));
    m_searchBox = new QLineEdit();
    m_searchBox->setPlaceholderText("Filter messages...");
    connect(m_searchBox, &QLineEdit::textChanged,
            this, &LogViewerWidget::onSearchTextChanged);
    toolbarLayout->addWidget(m_searchBox);

    toolbarLayout->addStretch();

    // Buttons
    m_clearButton = new QPushButton("Clear");
    connect(m_clearButton, &QPushButton::clicked,
            this, &LogViewerWidget::onClearClicked);
    toolbarLayout->addWidget(m_clearButton);

    m_saveButton = new QPushButton("Save to File");
    connect(m_saveButton, &QPushButton::clicked,
            this, &LogViewerWidget::onSaveClicked);
    toolbarLayout->addWidget(m_saveButton);

    mainLayout->addLayout(toolbarLayout);

    // Options bar
    QHBoxLayout* optionsLayout = new QHBoxLayout();

    m_autoScrollCheck = new QCheckBox("Auto-scroll");
    m_autoScrollCheck->setChecked(true);
    connect(m_autoScrollCheck, &QCheckBox::toggled,
            this, [this](bool checked) { m_autoScroll = checked; });
    optionsLayout->addWidget(m_autoScrollCheck);

    m_wordWrapCheck = new QCheckBox("Word wrap");
    m_wordWrapCheck->setChecked(false);
    connect(m_wordWrapCheck, &QCheckBox::toggled,
            this, [this](bool checked) {
                m_logDisplay->setLineWrapMode(checked ?
                                                  QTextEdit::WidgetWidth : QTextEdit::NoWrap);
            });
    optionsLayout->addWidget(m_wordWrapCheck);

    optionsLayout->addStretch();
    mainLayout->addLayout(optionsLayout);

    // Log display area
    m_logDisplay = new QTextEdit();
    m_logDisplay->setReadOnly(true);
    m_logDisplay->setFont(QFont("Consolas", 9));
    m_logDisplay->setLineWrapMode(QTextEdit::NoWrap);
    mainLayout->addWidget(m_logDisplay);
}

void LogViewerWidget::onLogEntryAdded(const LogEntry& entry)
{
    if (!matchesFilters(entry)) {
        return;
    }

    m_logDisplay->append(entry.toHtml());

    if (m_autoScroll) {
        QScrollBar* scrollBar = m_logDisplay->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

void LogViewerWidget::onFilterChanged()
{
    updateDisplay();
}

void LogViewerWidget::onClearClicked()
{
    Logger::instance().clear();
}

void LogViewerWidget::onSaveClicked()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Save Log File",
        QString("spyder_log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")),
        "Text Files (*.txt);;All Files (*.*)"
        );

    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);

        QList<LogEntry> logs = Logger::instance().getRecentLogs(10000);
        for (const LogEntry& entry : logs) {
            stream << entry.toString() << Qt::endl;
        }

        file.close();
        Logger::instance().info(LogCategory::UI, "LogViewer",
                                QString("Logs saved to: %1").arg(filename));
    }
}

void LogViewerWidget::onLevelFilterChanged(int index)
{
    int level = m_levelFilter->itemData(index).toInt();
    m_minLevelFilter = (level >= 0) ? static_cast<LogLevel>(level) : LogLevel::Trace;
    updateDisplay();
}

void LogViewerWidget::onCategoryFilterChanged(int index)
{
    int category = m_categoryCombo->itemData(index).toInt();
    m_categoryFilter = static_cast<LogCategory>(category);
    updateDisplay();
}

void LogViewerWidget::onSearchTextChanged(const QString& text)
{
    m_searchText = text;
    updateDisplay();
}

void LogViewerWidget::updateDisplay()
{
    m_logDisplay->clear();

    QList<LogEntry> logs = Logger::instance().getRecentLogs(1000);
    for (const LogEntry& entry : logs) {
        if (matchesFilters(entry)) {
            m_logDisplay->append(entry.toHtml());
        }
    }

    if (m_autoScroll) {
        QScrollBar* scrollBar = m_logDisplay->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

bool LogViewerWidget::matchesFilters(const LogEntry& entry) const
{
    // Level filter
    if (entry.level < m_minLevelFilter) {
        return false;
    }

    // Category filter
    int catFilter = static_cast<int>(m_categoryFilter);
    if (catFilter >= 0 && entry.category != m_categoryFilter) {
        return false;
    }

    // Search text filter
    if (!m_searchText.isEmpty()) {
        QString searchLower = m_searchText.toLower();
        if (!entry.message.toLower().contains(searchLower) &&
            !entry.source.toLower().contains(searchLower)) {
            return false;
        }
    }

    return true;
}
