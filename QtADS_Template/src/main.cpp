#include <DockFramework.h>
#include "SamplePanels.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata (used by QSettings for state persistence)
    QApplication::setApplicationName("QtADS_MasterTemplate");
    QApplication::setOrganizationName("QtADSTemplate");
    QApplication::setApplicationVersion("1.0.0");

    // Register all panel types before creating the window
    registerSamplePanels();

    // Create and show the dock main window
    DockManager::DockMainWindow window;
    window.setWindowTitle("QtADS Master Template");
    window.show();

    return app.exec();
}
