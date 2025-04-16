#ifndef WHSHOW_H
#define WHSHOW_H
#include <QFrame>
#include <QLabel>


class WHShow : public QFrame
{
    Q_OBJECT
public:
    WHShow();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel* label;
};

#endif // WHSHOW_H
