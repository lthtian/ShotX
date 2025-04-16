#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QWidget>
#include <QLabel>
#include "systeminfo.h"
#include "toolboxwindow.h"
#include "shadowlabel.h"
#include "imagedisplay.h"

static const int g_width = 4; //灵敏度, 像素大小

enum ScreenType
{
    Select = 0x0001,                                    // 选择截图矩形
    Move = 0x0002,                                      // 移动矩形区域
    SetSize = 0x0004,                                   // 设置矩形大小
    Paint = 0x0008,                                     // 绘画
    Mosaic = 0x0010                                     // 马赛克
};

Q_DECLARE_FLAGS(ScreenTypes, ScreenType)                // 枚举 ScreenType 生成宏 ScreenTypes
Q_DECLARE_OPERATORS_FOR_FLAGS(ScreenTypes)              // 重载宏 ScreenTypes 的 |() 函数

enum PosType
{
    Contains,                                           // 点在矩形内部
    Cross,                                              // 点在矩形线上
    External,                                           // 点在矩形外部
    Other                                               // 未知
};

enum PosArrow
{                                         // 鼠标在线框时候的方向枚举
    Top,
    Bottom,
    Left,
    Right,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    UnKnown
};


class ScreenShot : public QWidget
{
    Q_OBJECT
public:
    ~ScreenShot();

    static ScreenShot *GetInstanses();   //单例模式生成实例
    void init();
    QRect setCurrRect();                                // 获取当前截图矩形
    QRect setPaintRect();
    PosType isInArea(QPoint pos, int width = g_width);  // 判断鼠标是否在截图矩形之中, 也有实时更新鼠标的功能
    void setMouseIcon(QPoint pos, int width = g_width); // 细致判断鼠标图标
    PosArrow posArrow(QPoint pos, int width = g_width); // 在SetSize模式下用来判断调整方向的函数
    bool DectionAndSetMouseTracking(bool b = false);    // 鼠标跟踪开启与否
    QRect rootRect();                                   // 获取操作系统分辨率下的矩形大小
    QRect baseRect();                                   // 利用SystemInfo中的屏幕缩放比获取物理分辨率下实际矩形大小
    QPixmap savePixmap();                               // 保存时获取当前矩形图片
    void paintRect();                                   // 开启画矩形功能
    void paintMosaic();                                 // 开启画马赛克功能
    void closeMosaic();                                 // 关闭马赛克功能
    void reset();                                       // 使用截图或保存功能后将ShotRect复原
    void reset_paint();
    const QPixmap* MakeScreenPixmap();                  // 从SystemInfo中获取原图
    const QPixmap* MakeBackboardPixmap();               // 利用原图处理获得BackboardPixmap

    ppp getNailPictrue();                           // 从ScreenShot中返回当前截图矩形的ppp

    bool isStart = false;                      // 是否正在使用截图功能
    bool isUsingMosaic = false;                // 是否正在绘制马赛克

private:
    ScreenShot();                                       // 构造函数私有, 保持单例模式
    QPoint MoveOffset();                                // 计算移动操作位移的函数
    void updateCursor(QPoint pos, int width = 0);       // 根据位置更新鼠标显示的状态
    void drawScreenRect(QRect &rect, QPainter &pa);     // 绘画截图黑白线框
    void drawAnchor(QPainter &pa, bool b = true, int r = g_width); // 绘画线框的八个锚点
    PosArrow posarrow(QPoint pos, int width = g_width); // 根据位置判断当前的方向
    QImage addMosaicEffectToCircle(const QImage &srcImage, const QPoint &center, int radius, int blockSize);

protected:
    void keyPressEvent(QKeyEvent *event) override;      // 快捷键控制截图操作
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;         // 更新屏幕的主要事件


private:
    QPixmap* ScreenPixmap;             // 屏幕原图
    QPixmap* BackgroundPixmap;         // 截图时处理的背景图;
    //SystemInfo* sysinfo;               // 获取物理屏幕的基础信息(用于取出DPI适配屏幕)
    //ToolBoxWindow* MyToolBox;          // 截图区域右下角的工具栏

    //无操作时开启鼠标跟踪, 有操作时关闭
    bool isMouseTracking = false;              // 鼠标跟踪是否开启
    bool isMousePressed = false;               // 鼠标是否按下

    // 三种对截图操作 : 初始截图操作/截图移动操作/修改截图尺寸的操作
    // 三种操作分别对应三对起始点
    QPoint StaPos;
    QPoint EndPos;      // 管理截图矩形的大小
    QPoint MoveStaPos;
    QPoint MoveEndPos;  // 管理截图矩形在Move状态下的位移
    QPoint SizeStaPos;
    QPoint SizeEndPos;  // 管理

    //新增操作:绘画
    QPoint PaintStaPos;
    QPoint PaintEndPos;

    QPoint TranslatePoint;                // 记录某次操作的位移向量

    QRect ShotRect;                       // 实时更新的截图矩形
    QRect PaintRect;                      // 绘画状态下实时更新的矩形

    ScreenTypes MyActionType;             // 确定我本次操作的性质

    SystemInfo* systeminfo;               // 获取屏幕的基础信息

    ToolBoxWindow* MyToolBox;             // 设置工具栏

    ShadowLabel* label;                        // 尺寸展示

private:
    static ScreenShot* instance; // 保证一个单例
};

#endif // SCREENSHOT_H
