#ifndef TARYICONWINDON_H
#define TARYICONWINDON_H

#include <QSystemTrayIcon>
#include <QWidget>
#include <QMenu>

#include "screenshot.h"
#include "imagedisplay.h"

class ScreenShot;

class TrayIconWindow : public QWidget
{
    Q_OBJECT
public:
    TrayIconWindow();
    ~TrayIconWindow();

    void init();

public slots:
    //构造函数中实现初始化与槽函数
    void onIconActived(QSystemTrayIcon::ActivationReason reason);
    void toExit();

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

private:
    QSystemTrayIcon* MyTrayIcon;
    QMenu* MyMenu;
    QAction* ExitAction;

    ScreenShot* MyShot;
};

#endif // TARYICONWINDON_H
