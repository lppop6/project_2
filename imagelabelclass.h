#ifndef IMAGELABELCLASS_H
#define IMAGELABELCLASS_H

#include <QWidget>
#include <QDebug>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QLabel>
#include <QKeyEvent>
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QThread>
#include <screenshotthread.h>
#include <QVector>
#include <QScopedPointer>
#include <imageprocessingclass.h>

/*
 * 该类用于自定义Label控件，分为普通模式和特殊模式
 * 普通模式功能：
 * 按ctrl+滚轮可以将从MainWindow传进来的图片于鼠标位置进行放大缩小并传回MainWindow进行显示
 * 长按鼠标中键拖动可以拖动图片
 *
 * 特殊模式功能：
 * 面对畸变分析，可以预选取角点坐标，并画图显示。
 * 面对色差分析和畸变分析可以进行截屏操作，并显示截屏图片。
 */

class ImageLabelClass : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabelClass(QWidget *parent = nullptr);
    ~ImageLabelClass();

private:
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *ev);
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    virtual void resizeEvent(QResizeEvent *event);

    void reInitImagelabel();                                //当窗口大小变化的时候再次初始化label里面的事件


signals:
    //    void send_ZoomIn(QImage);
    //    void send_ZoomOut(QImage);
    void send_ZoomOrTransfer(QImage);
    void sendSpecialModeDraw(QImage,int,int,int,int);   //特殊模式画图信号，传递画图背景，四个顶点坐标
    void sendSpecialModeStyle(int);                     //特殊模式类型信号

    void sendFullColor(QImage,int,int,int,int);                //发送信号给thread使颜色覆盖角点搜索范围

    void sendReslt_Dis(double);                                       //发送结果给MainWindow,畸变
    void sendResult_Chro(double);                         //发送结果给MianWindow，色差
public slots:
    void get_Image(QImage);

    void Choose_Angular_Point();                            //角点范围选取模式开启

    void cutPicture();                                      //截图模式开启

    void chooseChromatic();                                 //选取色块模式

    void getMarkedImageFromThread(QImage);                  //从线程中获取标记后的图片（用于特殊模式）

    void dealThreadClose();                                 //绘画线程关闭进行的处理

    void quitSpecialMode();                                 //用于接受MainWindow的信号来退出特殊模式

    void completeScreenShot();                              //用于接受MainWindow的信号来完成截图并显示

    void firstPaintOnQImage(QImage);                          //用于导入图片时，对图片进行绘图，把图片放置再透明背景的中间部分

    double startTesting_Dis();                                   //开始进行畸变检测
    //void chromaticComplete();                               //色差选择完成

    QVector<int> startTesting_Chro();                         //开始进行色差检测
    void reChoosesAngular();                                  //重新选取四个顶点
private:
    QImage imageprocessed;                                                  //用于保存待处理的图像，就是从MainWindow那边来的图像
    QImage ss = QImage(1700 ,743 ,QImage::Format_ARGB32);             //透明的画布，1391和743是最大化状态的QImage
    //const QImage ss = QImage(2000,1000,QImage::Format_ARGB32_Premultiplied);

    //    const QImage ss = QImage(2000, 1000,QImage::Format::allGray());
    double multiple;                                       //缩放的倍数
    int imagewidth;
    int imageheight;                                       //图像的长和宽
    int positionx;
    int positiony;                                         //鼠标的位置

    double nextx2, nexty2;                                 //此处是图像的左上角顶点坐标

    int curpositionx;                                       //记录鼠标左键释放的位置(用于图像平移，普通模式)
    int curpositiony;
    int prepositionx;                                       //记录鼠标左键按压时的位置(用于图像平移，普通模式)
    int prepositiony;

    int curpositionx_1;                                     //记录鼠标左键释放和按压的位置(用于图片截取，选取色差位置之类的，就是特殊模式的操作)
    int curpositiony_1;
    int prepositionx_1;
    int prepositiony_1;
    int specialstyle;                                       //特殊模式类型

    //    int curlabelwidth;
    //    int curlabelheight;                                   //记录当前label的大小
    bool angularmode;                                       //用作判断是否为角点选择模式的bool值，true是为角点选择模式，false是为普通模式
    bool cutpicturemode;                                    //用于判断是否为截屏模式，用来截取图片的部分，并用来显示，true为截屏模式
    bool cutchromaticmode;                                  //用于判断是否为色差选取模式，用来截取色差测试图卡的各个色块的范围，用于后面的色差分析，true图卡色块选取模式
    bool isleftpress;                                       //用于判断左键是否进行按压，按压时的值为true，在左键释放的时候该位置位false

    bool isendmarked;                                       //用于判断是否确认本次角点范围
    bool isendmarked_1;                                     //用于判断本次角点范围确定后是否复制


    QImage tempcopy;                                        //作为拖动图像时用的复制件
    QImage drawcopy;                                        //作为标记特殊模式的背景复制件
    QImage angularcopy;                                     //作为角点范围选取的备份件，因为要选取四个角点
    QImage angulartempcopy;                                 //作为角点范围选取的临时备份件。

    ScreenShotThread* screenshotthread;                     //截屏的线程实例，并绘制截屏的区域
    QThread* shotthread;

    QVector<int> angularvector;                         //记录角点范围的数组
    QVector<int> chrovector;                            //记录色差块范围的数组


};

#endif // IMAGELABELCLASS_H
