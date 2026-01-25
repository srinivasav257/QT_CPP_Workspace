#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include "logger.h"

class LogViewerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogViewerWidget(QWidget* parent = nullptr);

private slots:
    void onLogEntryAdded(const LogEntry& entry);
    void onFilterChanged();
    void onClearClicked();
    void onSaveClicked();
    void onLevelFilterChanged(int index);
    void onCategoryFilterChanged(int index);
    void onSearchTextChanged(const QString& text);

private:
    void setupUI();
    void updateDisplay();
    bool matchesFilters(const LogEntry& entry) const;

    // UI Components
    QTextEdit* m_logDisplay;
    QComboBox* m_levelFilter;
    QComboBox* m_categoryCombo;
    QLineEdit* m_searchBox;
    QPushButton* m_clearButton;
    QPushButton* m_saveButton;
    QCheckBox* m_autoScrollCheck;
    QCheckBox* m_wordWrapCheck;

    // State
    LogLevel m_minLevelFilter;
    LogCategory m_categoryFilter;
    QString m_searchText;
    bool m_autoScroll;
};
