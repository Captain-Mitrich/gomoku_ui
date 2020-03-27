#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("Fusion");

    QColor backColor(16, 16, 0, 240);
    QColor gridColor(16, 96, 160);
    QColor cellColor(16, 140, 224);
    QColor line5Color(192, 16, 16);
    QColor grayColor(48, 48, 48);

    QPalette p = a.palette();
    p.setColor(QPalette::Window, backColor);
    p.setColor(QPalette::WindowText, gridColor);
    p.setColor(QPalette::Disabled, QPalette::WindowText, grayColor);
    p.setColor(QPalette::Base, Qt::transparent);
    p.setColor(QPalette::Button, backColor);
    p.setColor(QPalette::Text, cellColor);
    p.setColor(QPalette::ButtonText, cellColor);
    p.setColor(QPalette::Disabled, QPalette::ButtonText, grayColor);
    p.setColor(QPalette::Highlight, line5Color);
    p.setColor(QPalette::Dark, grayColor);
    p.setColor(QPalette::Shadow, grayColor);
    a.setPalette(p);

    GMainWindow w;
    w.show();

    return a.exec();
}
