#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QObject>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>
#include <QDebug>
#include <QImage>
//#include <videodataclass.h>
#include <QSharedPointer>
#include <QThread>
#include <QElapsedTimer>
#include <QWaitCondition>
#include <dataclass.h>
#include <QMessageBox>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

extern VideoState *videostate;


/*
 * 队列取数据包，解码传输mainwindow
 */

class PlayThread : public QObject
{
    Q_OBJECT
public:
    explicit PlayThread(QObject *parent = nullptr);
    ~PlayThread();

    void decode(int fps);

    void init();

    bool isStop();

signals:
    void send_CurTimeAndFrame(qint64,int);

    void send_Video(QImage, qint64);
public slots:
    void resume();

    void pause();

    void stop();

    void getNextFrame();

    void getPreFrame();

    void changePos(int);

    void gotoNext1sec();

    void gotoPre1sec();

    void gotoTest();

    void gotoPos(int, int);

    void normalVel();

    void slowVel();

private:
    QWaitCondition m_PauseManage;
    QSharedPointer<DataClass> Data_;
    QMutex temp_mutex;
    QMutex lll;
    bool isJump_Pause;
    bool isOK;
    bool isSlider;
    bool isNor;
};

#endif // PLAYTHREAD_H
