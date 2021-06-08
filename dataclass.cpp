#include "dataclass.h"

QSharedPointer<DataClass> DataClass::instance = QSharedPointer<DataClass>(new DataClass);


DataClass::DataClass()
{
    qDebug() << "DateClass构造";
    videostate = stream_Init();
    unuseSem.release();
}

void DataClass::packet_queue_flush(QueuePacket *q)
{
    AVPacketList *pkt1, *pkt2;
    QMutex t;
    t.lock();
    for(pkt1 = q->firstPacket; pkt1 != nullptr; pkt1 = pkt2){
        pkt2 = pkt1->next;
        av_free_packet(&pkt1->pkt);
        av_freep(&pkt1);
    }
    q->firstPacket = nullptr;
    q->lastPacket = nullptr;
    q->size = 0;
    t.unlock();
}

int DataClass::packet_queue_put(QueuePacket *q,AVPacket *pkt)
{
    AVPacketList* pkt1;
    pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
    if(!pkt1){
        return -1;
    }
    pkt1->pkt = *pkt;
    pkt1->next = nullptr;
    q->my_mutex->lock();
    if(!q->lastPacket){
        q->firstPacket = pkt1;
    }else{
        q->lastPacket->next = pkt1;
    }
    q->lastPacket = pkt1;
    q->size += pkt1->pkt.size;
    //设置条件量为有信号状态，进行唤醒
    q->my_mutex->unlock();
    return 0;
}

int DataClass::packet_queue_get(QueuePacket *q, AVPacket *pkt, int block)
{
    int ret = 1;
    AVPacketList *pkt1;
    q->my_mutex->lock();
    //只有当无法获得数据包的时候才会进行无限循环，这里不是CPU占用过高的原因
    while(true){
        if(q->abort_request){
            ret = -1;
            break;
        }
        pkt1 = q->firstPacket;
        if(pkt1){
            q->firstPacket = pkt1->next;
            if(!q->firstPacket){
                q->lastPacket = nullptr;
            }
            q->size -= pkt1->pkt.size;
            *pkt = pkt1->pkt;
            av_free(pkt1);
            ret = 1;
            break;
        }else if(!block){
            //非阻塞模式，无数据，直接退出
            ret = 0;
            break;
        }else{
            //阻塞模式，没数据就进入睡眠等待，在packet_queue_put中唤醒
            QThread::msleep(100);
            break;
        }
    }
    q->my_mutex->unlock();
    return ret;
}

VideoState *DataClass::stream_Init()
{
    QueuePacket *q = new QueuePacket();
    AVFrame *frame = av_frame_alloc();
    AVFrame *frame_RGB = av_frame_alloc();
    VideoState* is;
    is = (VideoState*)av_malloc(sizeof(VideoState));
    is->qpkt = q;
    is->isPause = false;
    is->isQuit = false;
    is->pFrame = frame;
    is->isJump = false;
    is->pFrameRGB = frame_RGB;
    is->decode_mutex = new QMutex();

    if(!is){
        return nullptr;
    }
    return is;
}

void DataClass::delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void DataClass::stream_delete()
{
    packet_queue_flush(videostate->qpkt);
    av_free(videostate);
}

void DataClass::stream_reInit()
{
    videostate = stream_Init();
}

void DataClass::clear_Queue()
{
    //    AVPacket *packet = (AVPacket *)  malloc ( sizeof (AVPacket));  //分配一个packet
    //    av_new_packet(packet, 10);
    //    strcpy (( char *)packet->data,FLUSH_DATA);
    packet_queue_flush(videostate->qpkt);
    //    packet_queue_put(videostate->qpkt, packet);  //往队列中存入用来清除的包
}
