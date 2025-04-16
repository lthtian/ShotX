#ifndef TOOLBOXWINDOW_H
#define TOOLBOXWINDOW_H
#include <QFrame>
#include <QToolButton>
#include "imagedisplay.h"

class ToolBoxWindow : public QFrame
{
    Q_OBJECT
public:
    ToolBoxWindow();
    void init();

    ToolBoxWindow* GetInstances();
    void resetMosaicIcon();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent* event) override;

private:
    void updateToolBtnIcon();

public slots:
    void Download();
    void Copy();
    void PaintRect();
    void Mosaic();
    void Nail();

private:
    bool isMosaicChecked = false; //判断是否开启马赛克功能

    QPoint TopLeftfPos; //相当于ShotRect
    QPoint StaPos;
    QPoint EndPos;
    QStringList ListName;
    QStringList ListToolTip;
    QVector<QToolButton*> var;

    ImageDisplay* MyDisplay;
};

#endif // TOOLBOXWINDOW_H
