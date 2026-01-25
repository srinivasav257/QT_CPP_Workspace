// cantracewindow.cpp
#include "cantracewindow.h"
#include "logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QDateTime>
#include <QScrollBar>

CANTraceWindow::CANTraceWindow(QWidget* parent)
    : QWidget(parent)
    , m_interface(nullptr)
    , m_database(nullptr)
    , m_isPaused(false)
    , m_autoScroll(true)
    , m_showTimestamp(true)
    , m_showDecoded(true)
    , m_maxMessages(10000)
    , m_channelFilterValue(-1)
    , m_messageCount(0)
    , m_startTime(0)
{
    setupUI();

    m_statsTimer = new QTimer(this);
    m_statsTimer->setInterval(1000);
    connect(m_statsTimer, &QTimer::timeout, this, &CANTraceWindow::updateStatistics);
    m_statsTimer->start();
}

CANTraceWindow::~CANTraceWindow()
{
}

void CANTraceWindow::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Toolbar
    QHBoxLayout* toolbarLayout = new QHBoxLayout();

    m_clearButton = new QPushButton("Clear");
    connect(m_clearButton, &QPushButton::clicked, this, &CANTraceWindow::onClearClicked);
    toolbarLayout->addWidget(m_clearButton);

    m_pauseButton = new QPushButton("Pause");
    m_pauseButton->setCheckable(true);
    connect(m_pauseButton, &QPushButton::clicked, this, &CANTraceWindow::onPauseClicked);
    toolbarLayout->addWidget(m_pauseButton);

    m_saveButton = new QPushButton("Save to File");
    connect(m_saveButton, &QPushButton::clicked, this, &CANTraceWindow::onSaveClicked);
    toolbarLayout->addWidget(m_saveButton);

    toolbarLayout->addSpacing(20);

    // Filters
    toolbarLayout->addWidget(new QLabel("Channel:"));
    m_channelFilter = new QComboBox();
    m_channelFilter->addItem("All", -1);
    m_channelFilter->addItem("Ch 0 (HS)", 0);
    m_channelFilter->addItem("Ch 1 (FD)", 1);
    connect(m_channelFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CANTraceWindow::onFilterChanged);
    toolbarLayout->addWidget(m_channelFilter);

    toolbarLayout->addWidget(new QLabel("ID Filter:"));
    m_idFilter = new QLineEdit();
    m_idFilter->setPlaceholderText("0x123 or 123");
    m_idFilter->setMaximumWidth(120);
    connect(m_idFilter, &QLineEdit::textChanged, this, &CANTraceWindow::onFilterChanged);
    toolbarLayout->addWidget(m_idFilter);

    toolbarLayout->addStretch();

    // Options
    m_autoScrollCheck = new QCheckBox("Auto-scroll");
    m_autoScrollCheck->setChecked(m_autoScroll);
    connect(m_autoScrollCheck, &QCheckBox::toggled, this, &CANTraceWindow::onAutoScrollToggled);
    toolbarLayout->addWidget(m_autoScrollCheck);

    m_showTimestampCheck = new QCheckBox("Timestamp");
    m_showTimestampCheck->setChecked(m_showTimestamp);
    connect(m_showTimestampCheck, &QCheckBox::toggled, this, &CANTraceWindow::onShowTimestampToggled);
    toolbarLayout->addWidget(m_showTimestampCheck);

    m_showDecodedCheck = new QCheckBox("Decode");
    m_showDecodedCheck->setChecked(m_showDecoded);
    connect(m_showDecodedCheck, &QCheckBox::toggled, this, &CANTraceWindow::onShowDecodedToggled);
    toolbarLayout->addWidget(m_showDecodedCheck);

    mainLayout->addLayout(toolbarLayout);

    // Statistics bar
    QHBoxLayout* statsLayout = new QHBoxLayout();
    m_statsLabel = new QLabel("Messages: 0 | Rate: 0 msg/s");
    m_statsLabel->setStyleSheet("font-weight: bold; color: #0066cc;");
    statsLayout->addWidget(m_statsLabel);
    statsLayout->addStretch();
    mainLayout->addLayout(statsLayout);

    // Trace table
    m_traceTable = new QTableWidget();
    m_traceTable->setColumnCount(7);
    m_traceTable->setHorizontalHeaderLabels({
        "#", "Timestamp", "Ch", "ID", "Name", "DLC", "Data"
    });

    m_traceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_traceTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_traceTable->setAlternatingRowColors(true);
    m_traceTable->verticalHeader()->setVisible(false);
    m_traceTable->setFont(QFont("Consolas", 9));

    // Column widths
    m_traceTable->setColumnWidth(0, 60);   // #
    m_traceTable->setColumnWidth(1, 120);  // Timestamp
    m_traceTable->setColumnWidth(2, 40);   // Ch
    m_traceTable->setColumnWidth(3, 80);   // ID
    m_traceTable->setColumnWidth(4, 200);  // Name
    m_traceTable->setColumnWidth(5, 40);   // DLC
    m_traceTable->horizontalHeader()->setStretchLastSection(true);

    connect(m_traceTable, &QTableWidget::cellClicked,
            this, &CANTraceWindow::onItemClicked);

    mainLayout->addWidget(m_traceTable);
}

void CANTraceWindow::setCANInterface(ICANInterface* interface)
{
    if (m_interface) {
        disconnect(m_interface, nullptr, this, nullptr);
    }

    m_interface = interface;

    if (m_interface) {
        connect(m_interface, &ICANInterface::messageReceived,
                this, &CANTraceWindow::onMessageReceived);
    }
}

void CANTraceWindow::setDatabase(DBCDatabase* database)
{
    m_database = database;
}

void CANTraceWindow::setAutoScroll(bool enable)
{
    m_autoScroll = enable;
    m_autoScrollCheck->setChecked(enable);
}

void CANTraceWindow::setMaxMessages(int max)
{
    m_maxMessages = max;
}

void CANTraceWindow::clearTrace()
{
    m_traceTable->setRowCount(0);
    m_messages.clear();
    m_messageCount = 0;
    m_startTime = QDateTime::currentMSecsSinceEpoch();

    LOG_DEBUG(LogCategory::CAN, "Trace cleared");
}

void CANTraceWindow::onMessageReceived(const CANMessage& msg)
{
    if (m_isPaused) {
        return;
    }

    if (!matchesFilter(msg)) {
        return;
    }

    m_messageCount++;

    // Store message
    m_messages.append(msg);

    // Limit buffer size
    while (m_messages.size() > m_maxMessages) {
        m_messages.removeFirst();
        if (m_traceTable->rowCount() > 0) {
            m_traceTable->removeRow(0);
        }
    }

    // Add to table
    addMessageToTable(msg);

    // Auto-scroll
    if (m_autoScroll) {
        m_traceTable->scrollToBottom();
    }
}

void CANTraceWindow::startRecording()
{
    m_isPaused = false;
    m_pauseButton->setChecked(false);
    m_startTime = QDateTime::currentMSecsSinceEpoch();

    LOG_INFO(LogCategory::CAN, "Recording started");
}

void CANTraceWindow::stopRecording()
{
    m_isPaused = true;
    m_pauseButton->setChecked(true);

    LOG_INFO(LogCategory::CAN, "Recording stopped");
}

void CANTraceWindow::pauseRecording()
{
    m_isPaused = !m_isPaused;
}

void CANTraceWindow::onClearClicked()
{
    clearTrace();
}

void CANTraceWindow::onPauseClicked()
{
    m_isPaused = m_pauseButton->isChecked();

    if (m_isPaused) {
        LOG_DEBUG(LogCategory::CAN, "Trace paused");
    } else {
        LOG_DEBUG(LogCategory::CAN, "Trace resumed");
    }
}

void CANTraceWindow::onSaveClicked()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Save CAN Trace",
        QString("can_trace_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")),
        "Text Files (*.txt);;CSV Files (*.csv);;All Files (*.*)"
        );

    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_ERROR(LogCategory::CAN, QString("Failed to save trace: %1").arg(filename));
        return;
    }

    QTextStream stream(&file);

    // Write header
    stream << "# CAN Trace - Saved at " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << Qt::endl;
    stream << "# Total Messages: " << m_messages.size() << Qt::endl;
    stream << Qt::endl;
    stream << "Timestamp,Channel,ID,Name,DLC,Data,Decoded" << Qt::endl;

    // Write messages
    for (const CANMessage& msg : m_messages) {
        QString decoded = decodeMessage(msg);
        stream << formatTimestamp(msg.timestamp) << ","
               << msg.channel << ","
               << QString("0x%1").arg(msg.id, 0, 16) << ","
               << (m_database ? (m_database->getMessage(msg.id) ? m_database->getMessage(msg.id)->name : "") : "") << ","
               << msg.dlc << ","
               << msg.dataToHex() << ","
               << decoded << Qt::endl;
    }

    file.close();

    LOG_INFO(LogCategory::CAN, QString("Trace saved: %1 messages to %2").arg(m_messages.size()).arg(filename));
}

void CANTraceWindow::onFilterChanged()
{
    m_channelFilterValue = m_channelFilter->currentData().toInt();
    m_idFilterText = m_idFilter->text().trimmed();

    emit filterChanged(m_idFilterText);
}

void CANTraceWindow::onAutoScrollToggled(bool checked)
{
    m_autoScroll = checked;
}

void CANTraceWindow::onShowTimestampToggled(bool checked)
{
    m_showTimestamp = checked;
    m_traceTable->setColumnHidden(1, !checked);
}

void CANTraceWindow::onShowDecodedToggled(bool checked)
{
    m_showDecoded = checked;
    // Refresh display
}

void CANTraceWindow::onItemClicked(int row, int column)
{
    if (row >= 0 && row < m_messages.size()) {
        emit messageSelected(m_messages[row]);
    }
}

void CANTraceWindow::addMessageToTable(const CANMessage& msg)
{
    int row = m_traceTable->rowCount();
    m_traceTable->insertRow(row);

    // Message number
    m_traceTable->setItem(row, 0, new QTableWidgetItem(QString::number(m_messageCount)));

    // Timestamp
    m_traceTable->setItem(row, 1, new QTableWidgetItem(formatTimestamp(msg.timestamp)));

    // Channel
    m_traceTable->setItem(row, 2, new QTableWidgetItem(QString::number(msg.channel)));

    // ID
    QString idStr = msg.isExtended ?
                        QString("0x%1").arg(msg.id, 8, 16, QChar('0')).toUpper() :
                        QString("0x%1").arg(msg.id, 3, 16, QChar('0')).toUpper();
    m_traceTable->setItem(row, 3, new QTableWidgetItem(idStr));

    // Name (from DBC)
    QString name = "";
    if (m_database) {
        const DBCMessage* dbcMsg = m_database->getMessage(msg.id);
        if (dbcMsg) {
            name = dbcMsg->name;
        }
    }
    m_traceTable->setItem(row, 4, new QTableWidgetItem(name));

    // DLC
    m_traceTable->setItem(row, 5, new QTableWidgetItem(QString::number(msg.dlc)));

    // Data
    QString dataStr = msg.dataToHex();
    if (m_showDecoded && m_database) {
        QString decoded = decodeMessage(msg);
        if (!decoded.isEmpty()) {
            dataStr += " | " + decoded;
        }
    }
    m_traceTable->setItem(row, 6, new QTableWidgetItem(dataStr));

    // Color coding
    if (msg.isRTR) {
        for (int col = 0; col < 7; ++col) {
            m_traceTable->item(row, col)->setBackground(QColor(255, 255, 200));
        }
    }
}

QString CANTraceWindow::formatTimestamp(uint64_t timestamp)
{
    double seconds = timestamp / 1000000.0;
    return QString::number(seconds, 'f', 6);
}

QString CANTraceWindow::decodeMessage(const CANMessage& msg)
{
    if (!m_database) {
        return "";
    }

    const DBCMessage* dbcMsg = m_database->getMessage(msg.id);
    if (!dbcMsg) {
        return "";
    }

    QMap<QString, QVariant> values = dbcMsg->parseData(msg.data, msg.dlc);

    QStringList parts;
    for (auto it = values.begin(); it != values.end(); ++it) {
        const DBCSignal* sig = dbcMsg->signals.value(it.key(), DBCSignal());

        QString valueStr;
        if (sig.valueTable.contains(it.value().toInt())) {
            valueStr = sig.valueTable[it.value().toInt()];
        } else {
            valueStr = QString::number(it.value().toDouble(), 'f', 2);
            if (!sig.unit.isEmpty()) {
                valueStr += " " + sig.unit;
            }
        }

        parts << QString("%1=%2").arg(it.key(), valueStr);
    }

    return parts.join(", ");
}

bool CANTraceWindow::matchesFilter(const CANMessage& msg)
{
    // Channel filter
    if (m_channelFilterValue >= 0 && msg.channel != m_channelFilterValue) {
        return false;
    }

    // ID filter
    if (!m_idFilterText.isEmpty()) {
        bool ok;
        uint32_t filterId;

        if (m_idFilterText.startsWith("0x", Qt::CaseInsensitive)) {
            filterId = m_idFilterText.mid(2).toUInt(&ok, 16);
        } else {
            filterId = m_idFilterText.toUInt(&ok, 16);
        }

        if (ok && msg.id != filterId) {
            return false;
        }
    }

    return true;
}

void CANTraceWindow::updateStatistics()
{
    if (m_startTime == 0) {
        m_startTime = QDateTime::currentMSecsSinceEpoch();
    }

    qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_startTime;
    double rate = 0.0;

    if (elapsed > 0) {
        rate = (m_messageCount * 1000.0) / elapsed;
    }

    m_statsLabel->setText(QString("Messages: %1 | Rate: %2 msg/s | Buffer: %3/%4")
                              .arg(m_messageCount)
                              .arg(rate, 0, 'f', 1)
                              .arg(m_messages.size())
                              .arg(m_maxMessages));
}
