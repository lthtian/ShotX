#include "whshow.h"
#include<QLabel>
#include"screenshot.h"

WHShow::WHShow()
{
    label = new QLabel(this);
    label->setText(QString(1111111));
    QPoint posNull;
    StaPos = EndPos = TopLeftfPos = posNull;
}

void WHShow::paintEvent(QPaintEvent *event)
{
    //从父控件获取StaPos的值, 通过这个值画label
    ScreenShot* screenshot = static_cast<ScreenShot*>(parent());


}




