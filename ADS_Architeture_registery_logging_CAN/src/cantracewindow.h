// cantracewindow.h
#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTimer>
#include "caninterface.h"
#include "dbcparser.h"

class CANTraceWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CANTraceWindow(QWidget* parent = nullptr);
    ~CANTraceWindow();

    void setCANInterface(ICANInterface* interface);
    void setDatabase(DBCDatabase* database);

    void setAutoScroll(bool enable);
    void setMaxMessages(int max);
    void clearTrace();

public slots:
    void onMessageReceived(const CANMessage& msg);
    void startRecording();
    void stopRecording();
    void pauseRecording();

signals:
    void messageSelected(const CANMessage& msg);
    void filterChanged(const QString& filter);

private slots:
    void onClearClicked();
    void onPauseClicked();
    void onSaveClicked();
    void onFilterChanged();
    void onAutoScrollToggled(bool checked);
    void onShowTimestampToggled(bool checked);
    void onShowDecodedToggled(bool checked);
    void onItemClicked(int row, int column);

private:
    void setupUI();
    void addMessageToTable(const CANMessage& msg);
    QString formatTimestamp(uint64_t timestamp);
    QString decodeMessage(const CANMessage& msg);
    bool matchesFilter(const CANMessage& msg);
    void updateStatistics();

    // UI Components
    QTableWidget* m_traceTable;
    QPushButton* m_clearButton;
    QPushButton* m_pauseButton;
    QPushButton* m_saveButton;
    QComboBox* m_channelFilter;
    QLineEdit* m_idFilter;
    QCheckBox* m_autoScrollCheck;
    QCheckBox* m_showTimestampCheck;
    QCheckBox* m_showDecodedCheck;
    QLabel* m_statsLabel;

    // Data
    ICANInterface* m_interface;
    DBCDatabase* m_database;
    QVector<CANMessage> m_messages;

    // Settings
    bool m_isPaused;
    bool m_autoScroll;
    bool m_showTimestamp;
    bool m_showDecoded;
    int m_maxMessages;
    QString m_idFilterText;
    int m_channelFilterValue;

    // Statistics
    uint64_t m_messageCount;
    uint64_t m_startTime;
    QTimer* m_statsTimer;
};
