#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>

class QEvent;
class QToolButton;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void toggleMaximizeRestore();

    QWidget *m_dragRegion = nullptr;
    QToolButton *m_maximizeButton = nullptr;
    bool m_dragActive = false;
    QPoint m_dragOffset;
};
#endif // MAINWINDOW_H
