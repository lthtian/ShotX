#include "imagedisplay.h"
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QDebug>

// 图钉事件逻辑:
// 点击图标保存此可框选的矩形图片, 退出截图模式, 将这个图片至于上层, 记录这个图片的范围
// 当鼠标进入范围中时, 对应图片会有额外阴影, 点击移动释放将会且仅会移动图片的方位
// 在该范围中的双击操作将被视为退出操作, 去除该图片并解除该范围

// 可以使用一个容器保存所有图钉图片及其方位, 在paintEvent中遍历
// 要加一个标记记录每个图片的叠加次序, 接受点击的图片应当置于其他图片顶端

ImageDisplay *ImageDisplay::instance = nullptr;

ImageDisplay *ImageDisplay::GetInstanses()
{
    if(instance == nullptr)
    {
        instance = new ImageDisplay();
    }
    instance->setCursor(Qt::ArrowCursor);
    return instance;
}

ImageDisplay::ImageDisplay(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground); // 透明背景
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);   // 窗口置顶 + 隐藏标题 + 不显示任务栏
    setFixedSize(QGuiApplication::primaryScreen()->size());               // 将窗口固定为全屏大小

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
}

void ImageDisplay::AddPPP(const ppp& newpicture)
{
    PictureBase.push_back(newpicture);
}

void ImageDisplay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    //检查当前鼠标在不在矩形中
    QPoint pos = QCursor::pos();
    bool flag = false;
    int index = PictureBase.size() - 1;
    for(auto picture = PictureBase.rbegin(); picture != PictureBase.rend(); ++picture)
    {
        QRect rect(picture->second.x(), picture->second.y(), picture->first->width(), picture->first->height());
        if(rect.contains(pos))
        {
            flag = true;
            break;
        }
        index--;
    }


    // 遍历并绘制所有 QPixmap
    int i = 0;
    for (const auto& picture : PictureBase) {
        if (picture.first) {  // 检查指针是否有效
            painter.drawPixmap(picture.second, *(picture.first)); // 在指定点绘制图片
            if(isMoving && i == PictureBase.size() - 1)
            {
                painter.save();
                QPen pen(QColor(211, 115, 13), 2);
                painter.setPen(pen);
                painter.drawRect(picture.second.x(), picture.second.y(), picture.first->width(), picture.first->height());
                painter.restore();
            }
            else if(!isMoving && i == index)
            {
                painter.save();
                QPen pen(QColor(227, 190, 14), 2);
                painter.setPen(pen);
                painter.drawRect(picture.second.x(), picture.second.y(), picture.first->width(), picture.first->height());
                painter.restore();
            }
            else
            {
                painter.drawRect(picture.second.x(), picture.second.y(), picture.first->width(), picture.first->height());
            }
        }
        i++;
    }
    //update();
}

void ImageDisplay::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "触发点击";
    // 并且通过按下的坐标分析按下的是那个图片的范围(从后往前遍历)
    // 如果选中了图片, 将该图片置顶, 开启移动图片模式, 否则无事发生
    QPoint pos = event->pos();
    bool flag = false;
    int index = PictureBase.size() - 1;
    for(auto picture = PictureBase.rbegin(); picture != PictureBase.rend(); ++picture)
    {
        QRect rect(picture->second.x(), picture->second.y(), picture->first->width(), picture->first->height());
        if(rect.contains(pos))
        {
            flag = true;
            PictureBase.push_back(*picture);
            PictureBase.erase(PictureBase.begin() + index);
            break;
        }
        index--;
    }
    if(flag)
    {
        //qDebug() << "找到图片";
        isMoving = true;
        MovePos = pos;
    }
    update();
}

void ImageDisplay::mouseMoveEvent(QMouseEvent *event)
{
    if(isMoving)
    {
        QPoint offset = event->pos() - MovePos;
        auto& picture = PictureBase[PictureBase.size() - 1];
        picture.second += offset;
        MovePos = event->pos();
    }
    update();
}

void ImageDisplay::mouseReleaseEvent(QMouseEvent *event)
{
    isMoving = false;
    update();
}

void ImageDisplay::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    int index = PictureBase.size() - 1;
    for(auto picture = PictureBase.rbegin(); picture != PictureBase.rend(); ++picture)
    {
        QRect rect(picture->second.x(), picture->second.y(), picture->first->width(), picture->first->height());
        if(rect.contains(pos))
        {
            PictureBase.erase(PictureBase.begin() + index);
            break;
        }
        index--;
    }
    update();
}



















