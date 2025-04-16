#include "toolboxwindow.h"
#include "screenshot.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QFileDialog>
#include <QDateTime>
#include <QClipboard>
#include <QApplication>

#define CURR_TIME QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")


ToolBoxWindow::ToolBoxWindow()
{
    init();
}

void ToolBoxWindow::init()
{
    setWindowFlags(Qt::FramelessWindowHint); // 去标题栏
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    //layout->
    StaPos = EndPos = QPoint();

    var.fill(nullptr, 5); // 先初始化数组
    ListName << "nail" << "mosaic" << "rect" << "download" << "copy";
    ListToolTip << tr("图钉") << tr("马赛克") << tr("绘制矩形") << tr("下载") << tr("拷贝到剪贴板");

    //遍历var构造每个按钮的名字, 创建QToolButton并设置参数, 将button和ListName/ListToolTip联系, 并且创建槽函数
    for(int i = 0; i < var.size(); i ++ )
    {
        QString name = ":/icons/" + ListName[i] + ".svg";
        var[i] = new QToolButton(this);
        var[i]->setIcon(QIcon(name));
        var[i]->setToolTip(ListToolTip[i]);
        var[i]->setAutoRaise(true);
        var[i]->setIconSize(QSize(24, 24));
        var[i]->setCheckable(false);

        layout->addWidget(var[i]);
    }

    connect(var[0], &QToolButton::released, this, &ToolBoxWindow::Nail);
    connect(var[1], &QToolButton::released, this, &ToolBoxWindow::Mosaic);
    connect(var[2], &QToolButton::released, this, &ToolBoxWindow::PaintRect);
    connect(var[3], &QToolButton::released, this, &ToolBoxWindow::Download);
    connect(var[4], &QToolButton::released, this, &ToolBoxWindow::Copy);
}

void ToolBoxWindow::resetMosaicIcon()
{
    var[1]->setIcon(QIcon(":/icons/mosaic.svg"));
}

void ToolBoxWindow::mousePressEvent(QMouseEvent *event)
{
    EndPos = StaPos = event->globalPos();
    TopLeftfPos = pos();
}

void ToolBoxWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(!StaPos.isNull())
    {
        EndPos = event->globalPos();
        move(TopLeftfPos + (EndPos - StaPos));
    }
}

void ToolBoxWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    EndPos = StaPos = QPoint();
}

void ToolBoxWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter pa(this);
    pa.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    pa.setPen(Qt::NoPen);
    pa.setBrush(QColor(255, 255, 255, 0.7 * 255));

    int round = 4;
    pa.drawRoundedRect(contentsRect().adjusted(1, 1, -1, -1), round, round);
}

void ToolBoxWindow::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    ScreenShot *screenshot = static_cast<ScreenShot*>(parent());
    screenshot->setCursor(Qt::ArrowCursor);
}


void ToolBoxWindow::Download()
{
    QString fileter = "图片文件(*.png);;图片文件(*.jpg);;所有文件(*.*)"; //文件过滤器
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存图片"), "ShotX" + CURR_TIME + ".png", fileter);
    ScreenShot* screenshot = static_cast<ScreenShot*>(parent());
    screenshot->savePixmap().save(CURR_TIME + ".png");
    if(!fileName.isEmpty())
    {
        screenshot->savePixmap().save(fileName);
    }
    screenshot->reset();
    screenshot->hide();
}

void ToolBoxWindow::Copy()
{
    ScreenShot *screenshot = static_cast<ScreenShot*>(parent());
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(screenshot->savePixmap());
    screenshot->reset();
    screenshot->hide();
}

// 框选矩形事件逻辑:
// 点击图标进入paint状态 -> paint状态点击设定框选矩形起点 -> 移动设置框选矩形大小 ->释放将当前框选矩形永久加入原图中
// paintEvent执行框选矩形的绘画工作
void ToolBoxWindow::PaintRect()
{
    ScreenShot *screenshot = static_cast<ScreenShot*>(parent());
    screenshot->paintRect();
}

// 马赛克事件逻辑:
// 点击图标进入mosaic状态,同时改变icon图标 -> mosaic状态鼠标变为一个圆
// -> 点击后直到释放都将把圆形内处理为马赛克(处理区域限制在shotrect内) -> 再次点击图标将状态恢复为select
// paintEvent改变鼠标的绘画逻辑
void ToolBoxWindow::Mosaic()
{
    ScreenShot *screenshot = static_cast<ScreenShot*>(parent());
    if(!isMosaicChecked)
    {
        isMosaicChecked = true;
        var[1]->setIcon(QIcon(":/icons/check.svg"));
        screenshot->paintMosaic();
    }
    else
    {
        isMosaicChecked = false;
        var[1]->setIcon(QIcon(":/icons/mosaic.svg"));
        screenshot->closeMosaic();
    }
}


// 图钉事件逻辑:
// 点击图标保存此可框选的矩形图片, 退出截图模式, 将这个图片至于上层, 记录这个图片的范围
// 当鼠标进入范围中时, 对应图片会有额外阴影, 点击移动释放将会且仅会移动图片的方位
// 在该范围中的双击操作将被视为退出操作, 去除该图片并解除该范围

// 可以使用一个容器保存所有图钉图片及其方位, 在paintEvent中遍历
// 要加一个标记记录每个图片的叠加次序, 接受点击的图片应当置于其他图片顶端

void ToolBoxWindow::Nail()
{
    ScreenShot *screenshot = static_cast<ScreenShot*>(parent());
    ppp data = screenshot->getNailPictrue();
    ImageDisplay::GetInstanses()->AddPPP(data);
    ImageDisplay::GetInstanses()->showFullScreen();
    screenshot->reset();
    screenshot->hide();
}



