#include "systeminfo.h"
#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>

SystemInfo::SystemInfo()
{
    MyScreen = QGuiApplication::primaryScreen();
}

int SystemInfo::logicalDPI()
{
    return MyScreen->logicalDotsPerInch();
}

int SystemInfo::physicalDPI()
{
    return MyScreen->physicalDotsPerInch();
}

int SystemInfo::devicePixelRatio()
{
    return MyScreen->devicePixelRatio(); //屏幕缩放比
}

//获取屏幕原图
QPixmap *SystemInfo::globalScreen()
{
    QPixmap* pix = new QPixmap(MyScreen->grabWindow(0));
    return pix;
}


