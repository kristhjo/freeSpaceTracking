#include "trackinggui.h"
#include <QApplication>
#include <QString>
#include <iostream>
int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    TrackingGui w;
    w.show();

    return a.exec();
}
