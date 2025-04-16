#ifndef IMAGEDISPLA_H
#define IMAGEDISPLA_H

#include <iostream>
#include <vector>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>

typedef std::pair<QPixmap*, QPoint> ppp;

class ImageDisplay : public QWidget
{
    Q_OBJECT
public:
    static ImageDisplay *GetInstanses();   //单例模式生成实例
    explicit ImageDisplay(QWidget *parent = nullptr);
    void AddPPP(const ppp& newpicture);

protected:
    void paintEvent(QPaintEvent *event) override;
    //void enterEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;


private:
    std::vector<ppp> PictureBase;         // 存储图钉图片的容器

    bool isMoving = false;                // 是否正在移动
    int MovePictureIndex = -1;            // 正在移动的图片下标
    QPoint MovePos;

    static ImageDisplay* instance; // 保证一个单例

    QGraphicsScene *scene;
    QGraphicsView *view;
};

#endif // IMAGEDISPLA_H
