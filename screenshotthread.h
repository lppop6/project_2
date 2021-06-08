#ifndef SCREENSHOTTHREAD_H
#define SCREENSHOTTHREAD_H

#include <QObject>
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QPalette>

/*
 * 用于截屏的线程，可以用于截取畸变分析和色差分析的图片，进而避免无关因素的干扰
 * 也用于畸变分析的辅助选取角点范围的功能
 */
class ScreenShotThread : public QObject
{
    Q_OBJECT
public:
    explicit ScreenShotThread(QObject *parent = nullptr);
    ~ScreenShotThread();
    void markQImage();                                          //对图像进行标记

    //void doAngularPoint(QImage &);                            //对角点选取范围进行的选取标记
signals:
    void sendMarkedPic(QImage);                                 //发送标记过的图片给ImageLabelClass
public slots:
    void getPointAndQImage(QImage,int,int,int,int);             //获取从ImageLabelClass获得的待处理图像和四个点位进而进行画图标记
    void getDrawStyle(int);                                     //获取从ImageLabelClass获得的Style

    void FullColor(QImage,int,int,int,int);                     //覆盖角点范围
    //void reAngular();                                         //重新记录角点范围。
private:
    int pre_x, cur_x, pre_y, cur_y, style;                      //四个顶点和截取的类型，依照不同的类型来进行标记不同的图案
    QImage imageprocessing;

};

#endif // SCREENSHOTTHREAD_H
