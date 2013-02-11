#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>
#include <QDebug>
#include "QtSerPro.h"

using namespace QtSerialPort;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
