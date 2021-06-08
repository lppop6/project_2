#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QWheelEvent>
#include <QPainter>
#include <QThread>
#include <QScopedPointer>
#include <inputresolutionresult.h>
#include <inputgrayscaleresult.h>
#include <ffmpegplayerclass.h>
#include <QMessageBox>
#include <delaytesting.h>
#include <subjecttesting.h>
#include <playthread.h>
#include <QTimer>
#include <deque>
#include <dataclass.h>
#include <inputyourname.h>
#include <makeexcle.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void image_Show(QImage);

    void importSubjectResult();

    void delay(int msec);

    void insertContainer(std::deque<std::pair<QImage,qint64>>&, std::pair<QImage,qint64>);

    double getChroResult(QVector<int>,QVector<int>);

    virtual void resizeEvent(QResizeEvent *event);

    void reSetExcel();              //重新提供表单

    void reSetLwtForm();            //重新清空界面的检测结果预览

    void reSetVideo();              //导入视频时参数重新置位
private slots:
    //下面是用于图像检测的槽
    void on_btn_ImportImage_clicked();

    void on_btn_ChooseAngularPoint_clicked();

    void on_btn_AngularPointComplete_clicked();

    void on_btn_RemapImage_clicked();

    void on_btn_ScreenShot_clicked();

    void on_btn_ScreenShotComplete_clicked();

    void on_btn_ChooseChromatic_clicked();

    void on_btn_ChromaticComplete_clicked();

    void on_btn_StartTestImage_clicked();

    void on_btn_ReChooseAngular_clicked();

    void on_rbn_Chromatic_clicked();

    void on_rbn_Resolution_clicked();

    void on_rbn_GrayScale_clicked();

    void on_rbn_Distortion_clicked();

    void get_Resolution_Result(double);

    void get_GrayScale_Result(double);

    //下面是用于视频检测的槽

    void on_btn_ImportVideo_clicked();

    void video_imshow(QImage,qint64);

    void on_btn_stopVideo_clicked();

    void on_btn_pauseVideo_clicked();

    void on_btn_resumeVideo_clicked();

    void on_btn_StartTestVideo_clicked();

    void get_VideoPara(qint64,int,int,int,double);      //获得总时长，帧频、码率、总帧数,时间基

    void get_VideoCurTimeAndFrame(qint64,int);          //获取当前时间和当前帧

    void deal_Close();                                  //处理关闭窗口的问题

    void on_btn_NextFrame_clicked();

    void on_btn_PreviousFrame_clicked();

    void update_Slider();

    void on_btn_goto_pre_5s_clicked();

    void on_btn_goto_next_5s_clicked();

    void on_btn_DefineHead_clicked();

    void on_btn_2X_clicked();

    void on_btn_1X_clicked();

    void on_btn_Locate_clicked();

    void on_btn_HandIn_clicked();

    void get_Your_Name(QString,QString,QString);

    void delThread();

signals:
    //下面是用于图像检测的信号
    void send_QImage_to_ImageLabel(QImage);

    void send_Resize_And_ChangeLabel();

    void send_AngularPoint();                       //角点点击按钮

    void send_QuitMode();                           //发送一个信号退出特殊模式

    void send_ScreenShot();                         //发送信号截图

    void send_SceeenShotComplete();                 //截图停止

    void send_Chromatic();                          //色差块选择信号

    void send_import_Image(QImage);                 //发送第一次导入的图片

    void send_ReChoseAngular();                     //重新选择角点

    //下面是用于视频检测的信号

    void send_putVideoPacket(QString);              //打开视频,获取数据包到队列

    void send_getVideoPacket(int fps);              //获取队列数据包并解码转换为RGB图像

    void send_StopVideo();

    void send_PauseVideo();

    void send_ResumeVideo();

    void send_ChangePos(int);                       //Qslider变化的信号，用于粗略定位

    void send_ChangePos_c(int);                     //用于精确定位视频位置的信号

private:
    Ui::MainWindow *ui;
    QImage testimage;                               //图片测试中的测试图像数据

    std::deque<std::pair<QImage, qint64>> image_container;                          //用做播放的图片动态存储器，用于纠正解码的顺序

    qint64 pretime;                                 //用作记录上次播放图片的ms时间---用来纠正解码顺序，转换为正确的播放顺序

    int labelwidth;
    int labelheight;                                //用来实现label，就是lbl_Image随着控件的尺寸大小不断变化的变量

    QImage orgin;                                   //初始图像，备份图片测试中的初始图像

    ffmpegPlayerClass* thread_ff;                   //构建一个获取待解码包到队列的线程
    PlayThread* thread_play;                        //构建一个解码显示RGB图像到lable的线程
    QThread* mythread_1;
    QThread* mythread_2;

    QTimer *timer;
    //用于数据的记录
    //图像：
    double grayScale;
    double distort;
    double resolution;
    double chromatic;
    QVector<int> Chro_1;
    QVector<int> Chro_2;
    bool isFirstChro;
    //视频：
    double totalTime;                                   //总时间长度 ms
    QString totalTime_QS;
    double curTime;                                     //当前时间
    double curTime_scale;                               //用于标记的时间
    int codeRate_r;
    int frameFre_r;                                     //用于记录表单
    int totalFrame;

    int frameFre;                                       //用于存储帧频值
    int codeRate;                                       //用于存储码率值
    int delayresu;
    bool isfluency;
    bool isjoggle;
    bool ismasic;
    bool isdisturb;
    bool isend;                                               //判断视频是不是结束？

    bool ispassSub;                 //用于判断是否进行过主观判断

    QString testerN;
    QString testingN;
    QString modelN;                 //用于表单的记录

    bool isdealclose;                                         //用作判断是不是已经析构过了

    qint64 time_delayget;                                     //这个是用来保存当前时间的变量，用于延时测试
    //bool isOver;
    double time_factor;                                       //视频的时间基

};

#endif // MAINWINDOW_H
