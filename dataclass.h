#ifndef DATACLASS_H
#define DATACLASS_H

#include <QObject>
#include <QImage>
#include <QPainter>
#include <QVector>
#include <QDebug>
#include <QTime>
#include <QMutex>
#include <QMutexLocker>
#include <QSharedPointer>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>
#include <QCoreApplication>
#include <QEventLoop>
#include <QThread>
#include <QSemaphore>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avstring.h>
}

#define FLUSH_DATA       "FLUSH"


struct QueuePacket                              //用于存储队列
{
    QueuePacket(){
        firstPacket = nullptr;
        lastPacket = nullptr;
        size = 0;
        abort_request = 0;
        my_mutex = new QMutex();
    }
    AVPacketList* firstPacket;                  //记录首个数据
    AVPacketList* lastPacket;                   //记录下Queue中最末尾的数据
    int size;                                   //队列的大小
    int abort_request;                          //异常请求
    QMutex* my_mutex;                           //队列的锁
};

//中转用的数据结构
struct VideoState
{
    QueuePacket *qpkt;                         //存储数据包的数据结构
    AVFrame *pFrame;                           //解码器解码之后的图像--YUV
    AVFrame *pFrameRGB;                        //存RGB数据
    AVFormatContext *pFormatCtx;               //存储音视频封装格式中包含的信息
    AVCodecContext	*pCodecCtx;                //视频流编码结构
    AVCodec *pCodec;                           //视频解码器
    uint8_t *out_buffer_rgb;                   //解码后的rgb数据
    struct SwsContext *img_convert_ctx;        //用于解码后的视频格式转换
    QMutex *decode_mutex;
    bool isPause;                              //用于判断是否暂停的信号量
    bool isQuit;                               //用于是否退出的信号量
    bool isJump;                               //用于跳到指定位置

    int seekFrame;                             //标志现在是哪一帧
    qint64 cur_time;                           //标记当前的时间，毫秒单位
    qint64 cur_pts;                            //标记当前的pts，播放时的pts -- 用于播放下一帧或上一帧这种
    qint64 seek_ms;                            //用于搜寻跳转位置---ms
    double seek_time;                          //用于搜寻跳转时间
    double time_change_factor;                 //时间基
    double fre_rate;                           //fps

};

class DataClass
{
public:
    DataClass();
    static QSharedPointer<DataClass> getInstance(){
        return instance;
    }
    VideoState* videostate;
public:
    void packet_queue_flush(QueuePacket*);
    int packet_queue_put(QueuePacket *,AVPacket *);
    int packet_queue_get(QueuePacket *, AVPacket *, int);
    VideoState* stream_Init();
    void delay(int);            //定义一个全局的函数
    void stream_delete();
    void stream_reInit();
    void clear_Queue();

    QSemaphore useSem;                      //已用量
    QSemaphore unuseSem;                    //未使用量
private:
    static QSharedPointer<DataClass> instance;

};
#endif // DATACLASS_H
