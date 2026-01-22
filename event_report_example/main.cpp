#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("EventTracking");
    MainWindow w;
    w.resize(800, 600);
    w.show();
    return a.exec();
}
