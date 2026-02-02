#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    QApplication::setApplicationName("QtTemplateApp");
    QApplication::setApplicationVersion("1.0.0");

    MainWindow window;
    window.show();

    return app.exec();
}
