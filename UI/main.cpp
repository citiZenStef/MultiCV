#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char **argv)
{
    QApplication app(argc,argv);

    MainWindow window;
    window.show();

    int retval = app.exec();

    return retval;
}
