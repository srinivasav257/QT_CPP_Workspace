#include "MainWindow.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Qt Professional Template");
    resize(800, 600);

    // Example widget
    QLabel *label = new QLabel("Hello from Professional Qt Template", this);
    label->setAlignment(Qt::AlignCenter);
    setCentralWidget(label);
}

MainWindow::~MainWindow()
{
}
