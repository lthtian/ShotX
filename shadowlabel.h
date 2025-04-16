#pragma once
#include <QApplication>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QWidget>

class ShadowLabel : public QLabel {
public:
    ShadowLabel(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }

protected:
     void paintEvent(QPaintEvent *event) override {
            // 先调用基类的绘制方法


            // 创建 QPainter 对象
            QPainter painter(this);

            // 设置字体
            QFont font = this->font();
            font.setWeight(QFont::Black);
            painter.setFont(font);

            // 设置阴影颜色
            QColor shadowColor(0, 0, 0, 160); // 黑色阴影，160 为透明度

            // 绘制阴影文本
            painter.setPen(shadowColor);
            painter.drawText(rect().adjusted(2, 2, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, text()); // 偏移位置绘制阴影

            // 绘制正常文本
            painter.setPen(QPalette().color(QPalette::WindowText)); // 使用标签的文本颜色
            painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, text()); // 正常文本

            QLabel::paintEvent(event);
        }
};
