#ifndef IDESHELLWINDOW_H
#define IDESHELLWINDOW_H

#include <QMainWindow>
#include <QPoint>

class QEvent;
class QLabel;
class QLineEdit;
class QToolButton;
class QWidget;

namespace ide_shell
{
class IdeShellWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit IdeShellWindow(QWidget *parent = nullptr);
    ~IdeShellWindow() override;

    QWidget *workspaceHost() const;
    void setWorkspaceWidget(QWidget *widget);
    void setStatusText(const QString &leftText, const QString &rightText);

protected:
    QWidget *createWelcomePanel() const;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void toggleMaximizeRestore();
    void buildShell();
    void applyShellStyle();

    QWidget *m_root = nullptr;
    QWidget *m_dragRegion = nullptr;
    QWidget *m_workspaceHost = nullptr;
    QToolButton *m_maximizeButton = nullptr;
    QLabel *m_leftStatusLabel = nullptr;
    QLabel *m_rightStatusLabel = nullptr;
    bool m_dragActive = false;
    QPoint m_dragOffset;
};
} // namespace ide_shell

#endif // IDESHELLWINDOW_H
