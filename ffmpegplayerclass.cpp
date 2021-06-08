#include "ffmpegplayerclass.h"

ffmpegPlayerClass::ffmpegPlayerClass(QObject *parent) : QObject(parent)
{
    qDebug() << "ffmpegPlayerClass构造";
    connect(this,&ffmpegPlayerClass::destroyed,this,&ffmpegPlayerClass::send_delete);
    Data_ = DataClass::getInstance();
}

ffmpegPlayerClass::~ffmpegPlayerClass()
{
    qDebug() << "ffmpegPlayerClass析构";
}

void ffmpegPlayerClass::resetPara()
{

}

//将数据包放入队列中
int ffmpegPlayerClass::putVideoPacket(QString path_Q)
{
    qDebug() << "ffmpegclass:" << QThread::currentThreadId();

    int numBytes;
    AVPacket *packet;

    std::string paths = path_Q.toStdString();
    const char* filepath = paths.c_str();                       //将文件路径转化成const char*

    av_register_all();                                          //注册ffmpeg库

    Data_.data()->videostate->pFormatCtx = avformat_alloc_context();          //初始化AVFormatContext
    if(avformat_open_input(&Data_.data()->videostate->pFormatCtx,filepath,nullptr,nullptr) != 0){ //打开视频流
        qDebug() << "不能打开流文件";
        return -1;
    }

    if(avformat_find_stream_info(Data_.data()->videostate->pFormatCtx,nullptr) < 0){      //获取视频流信息
        qDebug() << "无法找到视频流信息";
        return -1;
    }

    int videoindex = -1;            //视频帧索引
    for(unsigned int i = 0; i < Data_.data()->videostate->pFormatCtx->nb_streams; i++){        //查找视频流起始的索引位置，并查找打开视频编码器
        if(Data_.data()->videostate->pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoindex=i;
            break;
        }
    }

    if(videoindex == -1){
        qDebug() << "没有找到视频流 ";
        return -1;
    }

    //查找视频解码器
    Data_.data()->videostate->pCodecCtx = Data_.data()->videostate->pFormatCtx->streams[videoindex]->codec;
    Data_.data()->videostate->pCodec = avcodec_find_decoder(Data_.data()->videostate->pCodecCtx->codec_id);
    if(Data_.data()->videostate->pCodec == nullptr){
        qDebug() << "未发现解码器";
        return -1;
    }

    //qDebug() << "------=-=-=-=-=";

    //打开解码器
    if(avcodec_open2(Data_.data()->videostate->pCodecCtx, Data_.data()->videostate->pCodec, NULL) < 0){
        qDebug() << "无法找到解码器";
        return -1;
    }

    qDebug() << "----------------File Information--------------";
    av_dump_format(Data_.data()->videostate->pFormatCtx,0,filepath,0);
    qDebug() << "----------------------------------------------";

    /*
     *      当前帧的时间，码率，总时长，帧率，总帧数
     */
    AVStream *stream = Data_.data()->videostate->pFormatCtx->streams[videoindex];
    double frame_rate = 0;
    int code_rate = 0;
    int total_frame = 0;
    if(stream->codec->codec_type == AVMEDIA_TYPE_VIDEO){
        if(stream->avg_frame_rate.num != 0 && stream->avg_frame_rate.den != 0){
            frame_rate = stream->avg_frame_rate.num / stream->avg_frame_rate.den;//每秒多少帧
            Data_.data()->videostate->fre_rate = frame_rate;
        }
    }

    //获得该视频的时间基
    double factor= av_q2d(stream->time_base);
    factor = (1 / factor)/ 1000.0;
    qDebug() << "~~~~" << factor;
    Data_->videostate->time_change_factor = factor;


    code_rate = Data_.data()->videostate->pCodecCtx->bit_rate / 1000;
    qint64 total_time = Data_.data()->videostate->pFormatCtx->duration;
    qint64 usec = total_time / 1000000;
    total_frame = frame_rate * usec;
    emit send_VideoPara(total_time,frame_rate,code_rate,total_frame,factor);

    Data_.data()->videostate->pFrame = av_frame_alloc();              //创建帧结构，分配基本内存空间
    Data_.data()->videostate->pFrameRGB = av_frame_alloc();

    /**这里我们改成了 将解码后的YUV数据转换成RGB32**/

    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, Data_.data()->videostate->pCodecCtx->width, Data_.data()->videostate->pCodecCtx->height);

    Data_.data()->videostate->out_buffer_rgb = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) Data_.data()->videostate->pFrameRGB, Data_.data()->videostate->out_buffer_rgb, AV_PIX_FMT_RGB32,Data_.data()->videostate->pCodecCtx->width, Data_.data()->videostate->pCodecCtx->height);

    Data_.data()->videostate->img_convert_ctx = sws_getContext(Data_.data()->videostate->pCodecCtx->width, Data_.data()->videostate->pCodecCtx->height,
                                                               Data_.data()->videostate->pCodecCtx->pix_fmt, Data_.data()->videostate->pCodecCtx->width, Data_.data()->videostate->pCodecCtx->height,
                                                               AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    packet = (AVPacket*)av_malloc(sizeof(AVPacket));
    //将数据包放入队列中
    while(true)
    {
        if(Data_.data()->videostate->isQuit){
            qDebug() << "ff准备退出";

            break;
        }

        if(Data_.data()->videostate->isJump){
            //qDebug() << "可使用的unuse" << Data_.data()->unuseSem.available();
            //qDebug() << "可使用的use" << Data_.data()->useSem.available();
            Data_.data()->unuseSem.acquire();
            qDebug() << "here?";
            if((av_seek_frame(Data_.data()->videostate->pFormatCtx, videoindex, Data_.data()->videostate->seek_ms * Data_->videostate->time_change_factor , AVSEEK_FLAG_BACKWARD)) < 0)
            {
                qDebug() << "搜寻关键帧出错";
            }
            else
            {
                //qDebug() << "当前的pts:（ff）" << Data_.data()->videostate->cur_time * Data_->videostate->time_change_factor;
                //qDebug() << Data_.data()->videostate->cur_pts;
                //qDebug() << "我搜寻的时间是：" << Data_.data()->videostate->seek_ms * Data_->videostate->time_change_factor;
                //qDebug() << "搜寻当前pts附近的关键帧（ff）";
                if (videoindex >= 0)
                {
                    qDebug() << "清除Queue（ff）";
                    Data_.data()->clear_Queue();
                }
            }
            Data_.data()->useSem.release();
        }

        if(Data_.data()->videostate->qpkt->size > 56 * 1024){
            QThread::msleep(10);

            continue;
        }

        if(av_read_frame(Data_.data()->videostate->pFormatCtx,packet) >= 0){
            if(packet->stream_index == videoindex){
                Data_.data()->packet_queue_put(Data_.data()->videostate->qpkt,packet);
            }else{
                av_free_packet(packet);
            }
        }else{
            break;
        }
        //qDebug() << "ff";

        if(Data_.data()->videostate->isJump){
            QThread::msleep(3);
            qDebug() << "useSem(ff)" << Data_.data()->useSem.available();
            qDebug() << "unuseSem(ff)" << Data_.data()->unuseSem.available();
            Data_.data()->unuseSem.acquire();
            Data_.data()->videostate->isJump = false;
            //Data_.data()->videostate->isJumpOver = false;
            qDebug() << "isJump置位为false（ff）";
            Data_.data()->useSem.release();
        }

    }

    if(!Data_.data()->videostate->isQuit){
        QThread::msleep(1000);                               //延时让数据显示完
    }

    av_frame_free(&Data_.data()->videostate->pFrameRGB);
    av_frame_free(&Data_.data()->videostate->pFrame);
    avcodec_close(Data_.data()->videostate->pCodecCtx);
    avformat_close_input(&Data_.data()->videostate->pFormatCtx);
    return 0;
}

//弃用
void ffmpegPlayerClass::send_delete()
{
    qDebug() << "ffmpeg删除";
}




