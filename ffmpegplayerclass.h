#ifndef FFMPEGPLAYERCLASS_H
#define FFMPEGPLAYERCLASS_H

#include <QObject>
#include <QDebug>
#include <stdio.h>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QImage>
//#include <videodataclass.h>
#include <playthread.h>
#include <QSharedPointer>
#include <QThread>
#include <stdio.h>
#include <dataclass.h>
#include <QSemaphore>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

extern VideoState *videostate;

/*
 *  解码
 *  存解码之后的数据到VideoData里面的数据结构
 */

class ffmpegPlayerClass : public QObject
{
    Q_OBJECT
public:
    explicit ffmpegPlayerClass(QObject *parent = nullptr);
    ~ffmpegPlayerClass();

    void resetPara();

signals:
    void send_VideoPara(qint64,int,int,int,double);
public slots:
    int putVideoPacket(QString);
    void send_delete();
private:
    QSharedPointer<DataClass> Data_;

    QWaitCondition m_PauseManage_ff;
    QMutex temp_mutex_ff;
    QMutex ll;
};

#endif // FFMPEGPLAYERCLASS_H
