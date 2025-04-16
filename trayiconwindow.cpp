#include "trayiconwindow.h"
#include <QKeyEvent>
#include <QProcess>
#include "QDebug"

#include <windows.h>

#define HOTKEY_ID 1

TrayIconWindow::TrayIconWindow()
{
    this->setFocusPolicy(Qt::StrongFocus);

    //设置一个全局热键,用于直接启动截图功能
    if (RegisterHotKey((HWND)winId(), HOTKEY_ID, MOD_CONTROL | MOD_ALT, 0x5A)) // 0x5A 是 Z 的键值
    {
        qDebug() << "热键注册成功";
    }
    else
    {
        qDebug() << "热键注册失败";
    }

    //初始化
    init();

    //实现槽函数
    connect(MyTrayIcon, &QSystemTrayIcon::activated, this, &TrayIconWindow::onIconActived);
    connect(ExitAction, &QAction::triggered, this, &TrayIconWindow::toExit);
}

void TrayIconWindow::init()
{
    //ScreenShot = nullptr;
    //构造一个QSystemTrayIcon类型的对象
    //为这个对象添加QMenu和单击触发的函数
    MyTrayIcon = new QSystemTrayIcon(this);
    MyTrayIcon->setIcon(QIcon(":/icons/logo.svg"));
    MyTrayIcon->setToolTip(tr("TrayIconWindow"));

    //构造menu
    MyMenu = new QMenu(this);
    ExitAction = new QAction(tr("退出"));
    MyMenu->addAction(ExitAction);
    MyTrayIcon->setContextMenu(MyMenu);

    MyTrayIcon->show();
    this->setFocus();

    ImageDisplay::GetInstanses()->showFullScreen();
}

TrayIconWindow::~TrayIconWindow()
{
    UnregisterHotKey((HWND)winId(), HOTKEY_ID); //撤销全局热键
    MyTrayIcon->deleteLater();
}

void TrayIconWindow::onIconActived(const QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    //单机系统托盘图标触发截图功能
    case QSystemTrayIcon::Trigger:
        {
            MyShot = ScreenShot::GetInstanses();
            MyShot->show();
            break;
        }
    default:
        break;
    }
}

void TrayIconWindow::toExit()
{
    MyTrayIcon->hide();
    UnregisterHotKey((HWND)winId(), HOTKEY_ID); //撤销全局热键
    MyTrayIcon->deleteLater();
    qDebug() << "热键取消成功";
    // 直接杀掉进程
    QString processName = "ShotX.exe"; // 替换为你要终止的进程名
    QProcess process;
    process.start("taskkill", QStringList() << "/F" << "/IM" << processName);
    process.waitForFinished();
    qDebug() << "进程终止命令已发送";

}

//本地事件
bool TrayIconWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_HOTKEY) {
        if (msg->wParam == HOTKEY_ID) {
            qDebug() << "全局热键 Ctrl + Alt + Z 被按下";
            // 处理热键事件
            MyShot = ScreenShot::GetInstanses();
            if(!MyShot->isStart)
            {
                MyShot->isStart = true;
                MyShot->show();
            }
            return true;  // 返回 true 表示我们处理了此事件
         }
    }
    return QWidget::nativeEvent(eventType, message, result);
}

















