#include "mainwindow.h"
#include "trayiconwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    //开启高DPI模式
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    TrayIconWindow* TaryIcon = new TrayIconWindow();

    return a.exec();
}
