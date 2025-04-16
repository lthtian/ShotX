#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H
#include <QScreen>

class SystemInfo
{
public:
    SystemInfo();

    int logicalDPI();
    int physicalDPI();
    int devicePixelRatio();
    //QRect ScreenGeometry();
   // QRect availableGeometry();
    QPixmap *globalScreen();
private:
    QScreen* MyScreen;
};

#endif // SYSTEMINFO_H
