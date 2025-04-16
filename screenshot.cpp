#include "screenshot.h"
#include <QGuiApplication>
#include <QScreen>
#include <QIcon>
#include <QMouseEvent>
#include <QDebug>
#include <QtGlobal>
#include <QPainter>
#include <cmath>

ScreenShot* ScreenShot::instance = nullptr;

ScreenShot *ScreenShot::GetInstanses()
{
    if(instance == nullptr)
    {
        instance = new ScreenShot();
    }
    instance->setCursor(Qt::ArrowCursor);

    instance->MakeScreenPixmap();
    instance->MakeBackboardPixmap();
    return instance;
}

ScreenShot::ScreenShot()
{
    init();

    //生成info和toolbox
    systeminfo = new SystemInfo();
    MyToolBox = new ToolBoxWindow();
    MyToolBox->setParent(this);           //设置工具栏的父部件为ScreenShot, 确立从属关系

    label = new ShadowLabel(this);
    QFont font = label->font();  // 获取当前的字体
    font.setBold(true);          // 设置字体为粗体
    font.setPointSize(16);
    label->setFont(font);
    label->setFixedSize(QSize(300, 25));
    label->setText(QString("1"));

    //生成原图和背景图
    MakeScreenPixmap();
    MakeBackboardPixmap();
}

const QPixmap *ScreenShot::MakeScreenPixmap()
{
    ScreenPixmap = systeminfo->globalScreen();
    return ScreenPixmap;
}

const QPixmap *ScreenShot::MakeBackboardPixmap()
{
    if(!ScreenPixmap) return nullptr;

    QPixmap fillpixmap(ScreenPixmap->size());
    fillpixmap.fill(QColor(0, 0, 0, 0.4 * 255));
    BackgroundPixmap = new QPixmap(*ScreenPixmap);
    QPainter pa(BackgroundPixmap);
    pa.drawPixmap(BackgroundPixmap->rect(), fillpixmap);
    return BackgroundPixmap;
}

ppp ScreenShot::getNailPictrue()
{
    QPixmap* nailPicture = new QPixmap(ScreenPixmap->copy(ShotRect));
    ppp data = {nailPicture, ShotRect.topLeft()};
    return data;
}


ScreenShot::~ScreenShot()
{
    delete ScreenPixmap;
    delete BackgroundPixmap;
}

void ScreenShot::init()
{
    //初始化各种参数
    setObjectName("screenShots");
    setWindowTitle("ScreenShots");
    TranslatePoint = QPoint(0, 0);
    ShotRect = QRect();
    QPoint posNull;
    StaPos = EndPos = posNull;
    SizeStaPos = SizeEndPos = posNull;
    MoveStaPos = MoveEndPos = posNull;
    PaintStaPos = PaintEndPos = posNull;
    ScreenPixmap = BackgroundPixmap = nullptr;
    MyActionType = ScreenType::Select;

    //开启鼠标跟踪
    DectionAndSetMouseTracking(true);

    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);   // 窗口置顶 + 隐藏标题
    setFixedSize(QGuiApplication::primaryScreen()->size());               // 将窗口固定为全屏大小
    setWindowIcon(QIcon(":/icons/logo.svg"));
}

QRect ScreenShot::setCurrRect()
{
    if(StaPos == EndPos)
    {
        return QRect();
    }

    ShotRect.setX(qMin(StaPos.x(), EndPos.x()));
    ShotRect.setY(qMin(StaPos.y(), EndPos.y()));
    ShotRect.setWidth(qAbs(StaPos.x() - EndPos.x()));
    ShotRect.setHeight(qAbs(StaPos.y() - EndPos.y()));
    return ShotRect;
}

QRect ScreenShot::setPaintRect()
{
    if(PaintStaPos == PaintEndPos)
    {
        return QRect();
    }

    PaintRect.setX(qMin(PaintStaPos.x(), PaintEndPos.x()));
    PaintRect.setY(qMin(PaintStaPos.y(), PaintEndPos.y()));
    PaintRect.setWidth(qAbs(PaintStaPos.x() - PaintEndPos.x()));
    PaintRect.setHeight(qAbs(PaintStaPos.y() - PaintEndPos.y()));
    return PaintRect;
}

PosType ScreenShot::isInArea(QPoint pos, int width)
{
    if(ShotRect.isEmpty() || isMousePressed || MyActionType == Mosaic)
        return PosType::Other;

    int w = 2 * width;
    QRect outboard(ShotRect.x() - width, ShotRect.y() - width, ShotRect.width() + w, ShotRect.height() + w); //外侧矩形
    QRect inside(ShotRect.x() + width, ShotRect.y() + width, ShotRect.width() - w, ShotRect.height() - w);   //内侧矩形

    if(outboard.contains(pos, true))
    {
        if(inside.contains(pos, true))// 内部
        {
            if(MyActionType == ScreenType::Paint) setCursor(Qt::ArrowCursor);
            else setCursor(Qt::SizeAllCursor);
            return PosType::Contains;
        }
        else // 线上
        {
            if(isMouseTracking) setMouseIcon(pos, width);
            return PosType::Cross;
        }
    }
    else // 外部
    {
        setCursor(Qt::ArrowCursor);
        return PosType::External;
    }
}

void ScreenShot::setMouseIcon(QPoint pos, int width)
{
    if(ShotRect.isEmpty() || width < 1) return;

    int x1 = ShotRect.x();
    int y1= ShotRect.y();
    int x2 = ShotRect.x() + ShotRect.width();
    int y2 = ShotRect.y() + ShotRect.height();
    int w = width * 2;

    QRect rTopLeft(x1 - width, y1 - width, w, w);
    QRect rTopRight(x2 - width, y1 - width, w, w);
    QRect rBottomLeft(x1 - width, y2 - width, w, w);
    QRect rBottomRight(x2 - width, y2 - width, w, w);
    QRect rTop(x1 + width, y1 - width, ShotRect.width() - w, w);
    QRect rBottom(x1 + width, y2 - width, ShotRect.width() - w, w);
    QRect rLeft(x1 - width, y1 + width, w, ShotRect.height() - w);
    QRect rRight(x2 - width, y1 + width, w, ShotRect.height() - w);

    if (rTopLeft.contains(pos) || rBottomRight.contains(pos))
            setCursor(Qt::SizeFDiagCursor);
    if (rBottomLeft.contains(pos) || rTopRight.contains(pos))
        setCursor(Qt::SizeBDiagCursor);
    if (rTop.contains(pos) || rBottom.contains(pos))
        setCursor(Qt::SizeVerCursor);
    if (rLeft.contains(pos) || rRight.contains(pos))
        setCursor(Qt::SizeHorCursor);
}

PosArrow ScreenShot::posarrow(QPoint pos, int width)
{
    if (ShotRect.isEmpty() || width < 1)
            return PosArrow::UnKnown;

    int x1 = ShotRect.x();
    int y1 = ShotRect.y();
    int x2 = ShotRect.x() + ShotRect.width();
    int y2 = ShotRect.y() + ShotRect.height();
    int w = 2 * width;

    QRect rTopLeft(x1 - width, y1 - width, w, w);
    QRect rTopRight(x2 - width, y1 - width, w, w);
    QRect rBottomLeft(x1 - width, y2 - width, w, w);
    QRect rBottomRight(x2 - width, y2 - width, w, w);
    QRect rTop(x1 + width, y1 - width, ShotRect.width() - w, w);
    QRect rBottom(x1 + width, y2 - width, ShotRect.width() - w, w);
    QRect rLeft(x1 - width, y1 + width, w, ShotRect.height() - w);
    QRect rRight(x2 - width, y1 + width, w, ShotRect.height() - w);

    if (rBottomRight.contains(pos, true))
        return PosArrow::BottomRight;
    else if (rBottom.contains(pos, true))
        return PosArrow::Bottom;
    else if (rRight.contains(pos, true))
        return PosArrow::Right;
    else if (rTopRight.contains(pos, true))
        return PosArrow::TopRight;
    else if (rBottomLeft.contains(pos, true))
        return PosArrow::BottomLeft;
    else if (rTop.contains(pos, true))
        return PosArrow::Top;
    else if (rLeft.contains(pos, true))
        return PosArrow::Left;
    else if (rTopLeft.contains(pos, true))
        return PosArrow::TopLeft;
    else
        return PosArrow::UnKnown;
}



bool ScreenShot::DectionAndSetMouseTracking(bool b)
{
    isMouseTracking = b;
    setMouseTracking(isMouseTracking);
    return isMouseTracking;
}


void ScreenShot::mousePressEvent(QMouseEvent *event)
{
    //关闭鼠标追踪
    DectionAndSetMouseTracking(false);

    //按下时要确定本次操作的性质(是三种操作的哪一种)
    //内部就是移动, 外部就是重新选择矩形, 线上就是调节矩形大小
    if(ShotRect.isEmpty())
        MyActionType = ScreenType::Select;

    PosType postype = isInArea(event->pos());
    isMousePressed = true; // 在确认完性质后就不改变鼠标样式, 直到松开

    if(MyActionType == Select || MyActionType == Move || MyActionType == SetSize)
    {
        switch(postype)
        {
        case PosType::Contains :
            MyActionType = ScreenType::Move;
            break;
        case PosType::Cross :
            MyActionType = ScreenType::SetSize;
            break;
        case PosType::External :
            MyActionType = ScreenType::Select;
            break;
        case PosType::Other :
            break;
        default:
            qDebug() << "有问题";
            break;
        }
    }

    //根据正在进行的操作设置关键点
    if(MyActionType == ScreenType::Select)
        StaPos = EndPos = event->pos();
    else if(MyActionType == ScreenType::Move)
        MoveStaPos = MoveEndPos = event->pos();
    else if(MyActionType == ScreenType::SetSize)
        SizeStaPos = SizeEndPos = event->pos();
    else if(MyActionType == ScreenType::Paint)
        PaintEndPos = PaintStaPos = event->pos();
    else if(MyActionType == Mosaic)
        isUsingMosaic = true;
    update();
}

void ScreenShot::mouseMoveEvent(QMouseEvent *event)
{
    isInArea(event->pos(), g_width); // 这个函数在这起到刷新鼠标样式的作用

    //qDebug() << "触发MoveEvent";
    if(!isMouseTracking)
    {
        if(MyActionType == ScreenType::Select)
            EndPos = event->pos();
        else if(MyActionType == ScreenType::Move)
            MoveEndPos = event->pos();
        else if(MyActionType == ScreenType::SetSize)
            SizeEndPos = event->pos();
        else if(MyActionType == ScreenType::Paint)
            PaintEndPos = event->pos();
    }

    //如果在mosaic状态下, 修改圆范围内的清晰度
    if(isUsingMosaic)
    {
        QImage ScreenImage = ScreenPixmap->toImage();
        QPoint center = event->pos();
        //将修改清晰度的原图返回
        *ScreenPixmap = QPixmap::fromImage(addMosaicEffectToCircle(ScreenImage, center, 13, 2));
    }
    update();
}

void ScreenShot::mouseReleaseEvent(QMouseEvent *event)
{
    isMousePressed = false;
    if(MyActionType == ScreenType::Select)
        EndPos = event->pos();
    else if(MyActionType == ScreenType::Move)
    {
        StaPos = ShotRect.topLeft();
        EndPos = ShotRect.bottomRight() + QPoint(1, 1); // 半开区间,历史原因
        MoveEndPos = MoveStaPos = QPoint(0, 0);
    }
    else if(MyActionType == ScreenType::SetSize)
    {
        StaPos = ShotRect.topLeft();
        EndPos = ShotRect.bottomRight() + QPoint(1, 1);
        SizeEndPos = event->pos();
    }
    else if(MyActionType == ScreenType::Paint)
    {
        //将这个矩形彻底融入原图
        QPainter pa(ScreenPixmap);
        QPen pen;
        pen.setColor(QColor(255, 0, 0));
        pen.setWidth(2);
        pa.setPen(pen);
        pa.drawRect(PaintRect);

        //一次直接退出绘画模式
        PaintEndPos = PaintStaPos = QPoint();
        MyActionType = ScreenType::Select;
    }
    else if(MyActionType == ScreenType::Mosaic)
    {
        isUsingMosaic = false;
    }

    //开启鼠标追踪
    DectionAndSetMouseTracking(true);
    update();
}

void ScreenShot::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        reset();
        hide();
    }
    update();
}

//核心函数, 绘画截图区域, 并根据操作类型改变绘画策略
void ScreenShot::paintEvent(QPaintEvent *event)
{
    //qDebug() << "触发paintEvent";
    Q_UNUSED(event)
    if(!ScreenPixmap)
    {
        return;
    }

    QPainter pa(this);
    pa.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);   // 抗锯齿 + 平滑边缘处理

    //依据StaPos和EndPos更新当前的截图矩形
    setCurrRect();
    if(MyActionType == ScreenType::Paint) setPaintRect();

    //SetSize操作
    //根据获取posArrow来确定改变大小的方式
    QPoint pos = SizeEndPos - SizeStaPos;
    if(!pos.isNull() && MyActionType == ScreenType::SetSize)
    {
        switch(posarrow(SizeStaPos))//修改方向始终与开始修改的方向一致
        {
        case PosArrow::BottomRight:
            ShotRect.setBottomRight(ShotRect.bottomRight() + pos);
            break;
        case PosArrow::BottomLeft:
            ShotRect.setBottomLeft(ShotRect.bottomLeft() + QPoint(0, 1) + pos);
            break;
        case PosArrow::TopLeft:
            ShotRect.setTopLeft(ShotRect.topLeft() + pos);
            break;
        case PosArrow::TopRight:
            ShotRect.setTopRight(ShotRect.topRight() + pos);
            break;
        case PosArrow::Bottom:
            ShotRect.setBottom(ShotRect.bottom() + 1 + pos.y());
            break;
        case PosArrow::Right:
            ShotRect.setRight(ShotRect.right() + 1 + pos.x());
            break;
        case PosArrow::Top:
            ShotRect.setTop(ShotRect.top() + pos.y());
            break;
        case PosArrow::Left:
            ShotRect.setLeft(ShotRect.left() + pos.x());
            break;
        default:
            //qDebug() << "666";
            break;
        }
    }

    //Move操作
    QPoint Shift = MoveOffset(); // 计算出位移
    if(!Shift.isNull() && MyActionType == ScreenType::Move)
    {
        //利用位移直接移动截图矩形
        ShotRect.setTopLeft(ShotRect.topLeft() + Shift);
        ShotRect.setBottomRight(ShotRect.bottomRight() + Shift);
    }



    //共有操作
    pa.drawPixmap(QGuiApplication::primaryScreen()->geometry(), *BackgroundPixmap);//画背景
    if(!ShotRect.isEmpty())
    {
        pa.drawPixmap(ShotRect, ScreenPixmap->copy(baseRect()));  //画截图矩形
    }

    //Paint操作
    if(MyActionType == ScreenType::Paint && !PaintRect.isEmpty())
    {
        pa.save();
        QPen pen;
        pen.setColor(QColor(255, 0, 0));
        pa.setPen(pen);
        pa.drawRect(PaintRect);
        pa.restore();
    }

    drawScreenRect(ShotRect, pa); // 截图矩形画边
    drawAnchor(pa, true, 4); // 画八个锚点

    //共有操作
    //处理工具栏
    MyToolBox->move(ShotRect.right() - MyToolBox->width(), ShotRect.bottom() + g_width + 1);
    if(ShotRect.isValid())
        MyToolBox->show();
    else MyToolBox->hide();

    //画尺寸栏
    if(ShotRect.isValid())
    {
        //计算宽高
        int width = qAbs(ShotRect.topRight().x() - ShotRect.topLeft().x());
        int height = qAbs(ShotRect.bottomLeft().y() - ShotRect.topLeft().y());

        //qDebug() << "宽:"<<  width << " 高:" <<height;

        QString w = QString::number(width);
        QString h = QString::number(height);
        //qDebug() << "str:" << h;
        QString ret = w + " X " + h;
        label->setText(ret);
        label->setStyleSheet("color : white");
        label->move(ShotRect.topLeft() - (QPoint(0, label->height()) + QPoint(-7, 5)));
        label->show();
    }
    else label->hide();


    //Mosaic操作
    if(MyActionType == ScreenType::Mosaic)
    {
        //取得鼠标的位置, 以该位置为圆心画圆
        QPoint pos = QCursor::pos();
        pa.drawEllipse(pos.x() - 15, pos.y() - 13, 26, 26);
    }

    //update();
}

QRect ScreenShot::baseRect()
{
    return QRect(ShotRect.topLeft() * systeminfo->devicePixelRatio(), ShotRect.size() * systeminfo->devicePixelRatio());
}

QPixmap ScreenShot::savePixmap()
{
    return QPixmap(ScreenPixmap->copy(ShotRect));
}

void ScreenShot::paintRect()
{
    // 此处开启绘画状态, 事件函数有不同的处理方式
    reset_paint();
    MyActionType = Paint;
}

void ScreenShot::paintMosaic()
{
    MyActionType = Mosaic;
    setCursor(Qt::BlankCursor);
}

void ScreenShot::closeMosaic()
{
    MyActionType = Select;
    setCursor(Qt::ArrowCursor);
}

void ScreenShot::reset()
{
    ShotRect = QRect();
    QPoint posNull;
    StaPos = EndPos = posNull;
    SizeStaPos = SizeEndPos = posNull;
    MoveStaPos = MoveEndPos = posNull;
    isStart = false;
    MyActionType = Select;
    MyToolBox->resetMosaicIcon();
}

void ScreenShot::reset_paint()
{
    PaintRect = QRect();
    QPoint posNull;
    PaintStaPos = PaintEndPos = posNull;
}

QPoint ScreenShot::MoveOffset()
{
    return (MoveEndPos - MoveStaPos);
}

void ScreenShot::drawScreenRect(QRect &rect, QPainter &pa)
{
    pa.save();

    pa.setBrush(Qt::NoBrush);
    pa.setRenderHint(QPainter::Antialiasing, false);
    QPen penWhite(QColor(255, 255, 255, 255), 1);
    penWhite.setStyle(Qt::CustomDashLine);    //设置虚线
    penWhite.setDashOffset(0);                //设置白线的偏移量为0
    penWhite.setDashPattern(QVector<qreal>() << 4 << 4); //设置虚线样式
    penWhite.setCapStyle(Qt::FlatCap);        //设置线端点为平头
    pa.setPen(penWhite);
    pa.drawLine(QPoint(rect.left(), rect.top()), QPoint(rect.right(), rect.top()));
    pa.drawLine(QPoint(rect.left(), rect.top()),QPoint(rect.left(), rect.bottom()));
    pa.drawLine(QPoint(rect.left(), rect.bottom()), QPoint(rect.right(), rect.bottom()));
    pa.drawLine(QPoint(rect.right(), rect.top()),QPoint(rect.right(), rect.bottom()));

    QPen penBlack(penWhite);
    penBlack.setColor(QColor(0, 0, 0, 255));
    penWhite.setDashOffset(4);                //设置黑线的偏移量为4
    pa.setPen(penBlack);

    pa.drawLine(QPoint(rect.left(), rect.top()), QPoint(rect.right(), rect.top()));
    pa.drawLine(QPoint(rect.left(), rect.top()),QPoint(rect.left(), rect.bottom()));
    pa.drawLine(QPoint(rect.left(), rect.bottom()), QPoint(rect.right(), rect.bottom()));
    pa.drawLine(QPoint(rect.right(), rect.top()),QPoint(rect.right(), rect.bottom()));

    pa.restore();
}

void ScreenShot::drawAnchor(QPainter &pa, bool b, int r) //是否设置锚点/锚点半径
{
    //确定8个点, 以点为中心画矩形
    if(!b) return;

    pa.save();
    pa.setRenderHint(QPainter::Antialiasing, false);

    int x1 = ShotRect.left();
    int y1 = ShotRect.top();
    int x2 = ShotRect.right();
    int y2 = ShotRect.bottom();

    QVector<QPoint> ver;
    ver << QPoint(x1, y1);
    ver << QPoint(x2, y2);
    ver << QPoint(x1, y2);
    ver << QPoint(x2, y1);
    ver << QPoint((x1 + x2) >> 1, y1);
    ver << QPoint((x1 + x2) >> 1, y2);
    ver << QPoint(x1, (y1 + y2) >> 1);
    ver << QPoint(x2, (y1 + y2) >> 1);

    pa.setPen(QPen(Qt::white, 1.5));
    pa.setBrush(QColor(146, 146, 146, 1 * 255));

    foreach(QPoint e, ver)
        pa.drawEllipse(e, r, r);

    pa.restore();
}


//修改图片像素大小
QImage ScreenShot::addMosaicEffectToCircle(const QImage &srcImage, const QPoint &center, int radius, int blockSize) {
    QImage mosaicImage = srcImage;  // 复制原始图像

    // 遍历图像，只对圆形区域内的像素进行处理
    for (int y = center.y() - radius; y <= center.y() + radius; y += blockSize) {
        for (int x = center.x() - radius; x <= center.x() + radius; x += blockSize) {
            // 判断该像素块是否在圆形区域内
            if (std::pow(x - center.x(), 2) + std::pow(y - center.y(), 2) <= std::pow(radius + 3, 2)) {
                int sumR = 0, sumG = 0, sumB = 0;
                int pixelsInBlock = 0;

                // 计算每个块内的平均颜色
                for (int j = 0; j < blockSize; ++j) {
                    for (int i = 0; i < blockSize; ++i) {
                        int px = x + i;
                        int py = y + j;

                        QColor color = srcImage.pixelColor(px, py);
                        sumR += color.red();
                        sumG += color.green();
                        sumB += color.blue();
                        pixelsInBlock++;
                    }
                }

                // 计算块的平均颜色
                if (pixelsInBlock > 0) {
                    QColor avgColor(sumR / pixelsInBlock, sumG / pixelsInBlock, sumB / pixelsInBlock);

                    // 将平均颜色填充回块区域
                    for (int j = 0; j < blockSize; ++j) {
                        for (int i = 0; i < blockSize; ++i) {
                            int px = x + i;
                            int py = y + j;
                            mosaicImage.setPixelColor(px, py, avgColor);
                        }
                    }
                }
            }
        }
    }

    return mosaicImage;
}

//int main(int argc, char *argv[]) {
//    QApplication a(argc, argv);

//    // 加载图片
//    QPixmap pixmap(":/images/sample.jpg");
//    QImage image = pixmap.toImage();  // 转换为QImage

//    // 指定马赛克处理的圆形区域 (例如：圆心(150, 150)，半径50)
//    QPoint center(150, 150);
//    int radius = 50;
//    int blockSize = 10;  // 马赛克块的大小

//    // 添加马赛克效果，仅处理圆形区域
//    QImage mosaicImage = addMosaicEffectToCircle(image, center, radius, blockSize);

//    // 将处理后的图像显示在QLabel中
//    QLabel label;
//    label.setPixmap(QPixmap::fromImage(mosaicImage));
//    label.setFixedSize(pixmap.size());
//    label.show();

//    return a.exec();
//}

























