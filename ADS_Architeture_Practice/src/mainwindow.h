#pragma once

#include <QMainWindow>
#include <QMap>

namespace ads {
class CDockManager;
class CDockWidget;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    ads::CDockManager* m_dockManager = nullptr;
    QMap<QString, ads::CDockWidget*> m_docks;

    void registerDock(ads::CDockWidget* dock);
    void DockCreation(void);
};

