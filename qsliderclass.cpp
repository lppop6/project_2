#include "qsliderclass.h"

QSliderClass::QSliderClass(QWidget *parent) : QSlider(parent)
{
    isDrag = false;
}

//鼠标释放事件
void QSliderClass::mouseReleaseEvent(QMouseEvent *event)
{
    QSlider::mouseReleaseEvent(event);
    qDebug() << "鼠标释放";
    if(event->button() == Qt::LeftButton){
        emit send_changePos();
        emit send_moveSlide();              //发送一个移动鼠标的信号，这个信号是用来标志（如果现在是暂停播放的情况那么就把按钮置位为播放状态）
        QThread::msleep(100);
        isDrag = false;
    }
}

//鼠标按压
void QSliderClass::mousePressEvent(QMouseEvent *ev)
{
    qDebug() << "鼠标按压";
    QSlider::mousePressEvent(ev);
    if(ev->button() == Qt::LeftButton){
        isDrag = true;
    }
}

//鼠标拖拽
bool QSliderClass::mouseDrag()
{
    return isDrag;
}
