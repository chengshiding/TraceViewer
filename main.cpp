#include <QApplication>
#include <QTextCodec>
#include <QTextStream>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("myapp.qm",QCoreApplication::applicationDirPath());
    a.installTranslator(&qtTranslator);

    QTranslator qtTranslator2;
    qtTranslator2.load("qt_zh_CN.qm",QCoreApplication::applicationDirPath());
    a.installTranslator(&qtTranslator2);

    MainWindow w;
    w.minimumSizeHint();
    w.showMaximized();

    return a.exec();
}
