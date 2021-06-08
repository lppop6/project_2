#ifndef QSLIDERCLASS_H
#define QSLIDERCLASS_H

#include <QWidget>
#include <QSlider>
#include <QMouseEvent>
#include <QDebug>
#include <QThread>

class QSliderClass : public QSlider
{
    Q_OBJECT
public:
    explicit QSliderClass(QWidget *parent = nullptr);

    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent* ev);
    bool mouseDrag();
signals:

    void send_changePos();
    void send_moveSlide();
public slots:

private:
    bool isDrag;
};

#endif // QSLIDERCLASS_H
