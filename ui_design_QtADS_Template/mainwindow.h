#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <IdeShell/IdeShellWindow.h>

namespace ads
{
class CDockManager;
}

class MainWindow : public ide_shell::IdeShellWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void setupDockingArea();

    ads::CDockManager *m_dockManager = nullptr;
};

#endif // MAINWINDOW_H
