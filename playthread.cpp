#include "playthread.h"

PlayThread::PlayThread(QObject *parent) : QObject(parent)
{
    qDebug() << "PlayThread启动！！！";
    Data_ = QSharedPointer<DataClass> (DataClass::getInstance());
    isJump_Pause = false;                                           //用于判断跳转后是否暂停
    isOK = false;                                                   //用于判断跳转后是否暂停
    isSlider = false;                                               //用于判断是否是slider滑动触发的跳转事件
    isNor = true;
}

PlayThread::~PlayThread()
{
    qDebug() << "PlayThread析构";
}

//解码
void PlayThread::decode(int fps)
{
    /**获取数据包**/
    QThread::msleep(500);
    qDebug() << "fps" << fps;
    fps = 1000 / fps;
    fps = fps - 3;                      //转换格式+解码大概消耗3ms
    qDebug() << "decode:" << QThread::currentThreadId();
    int ret;
    int got_picture;
    AVPacket *packet;
    packet = (AVPacket*)av_malloc(sizeof(AVPacket));
    QImage tmpImg;
    QImage image;                       
    QImage preImage;                    //用于记录上一次解码的图片（用于av_seek_frame后偶然出现的decode失败的现象）
    bool isBadPacket = false;
    while(true){
        //qDebug() << "player";
        if(isBadPacket){
            qDebug() << "静帧";
            qint64 seek = Data_.data()->videostate->seek_ms;
            double factor = Data_->videostate->time_change_factor;
            double fps = Data_.data()->videostate->fre_rate;
            qDebug() << "静帧搜索的帧数：" << seek / (1000 / fps);
            emit send_CurTimeAndFrame(seek, seek / (1000 / fps));
            emit send_Video(preImage, seek);
            Data_.data()->videostate->cur_time = seek;
            Data_.data()->videostate->cur_pts = seek * factor;
            isBadPacket = false;

            if(isOK){
                Data_.data()->useSem.acquire();
                isOK = false;
                Data_.data()->unuseSem.release();
            }
            if(isJump_Pause ){
                Data_.data()->videostate->isPause = true;
                isJump_Pause = false;
            }
        }

        if(Data_.data()->videostate->isQuit){
            //qDebug() << "我是准备跳出去了--player";
            break;
        }
        //qDebug() << "当前帧数：" << Data_.data()->videostate->pCodecCtx->frame_number;
        if(Data_.data()->videostate->isPause){
            qDebug() << "暂停";
            temp_mutex.lock();
            m_PauseManage.wait(&temp_mutex);
            temp_mutex.unlock();
        }

        if(Data_.data()->videostate->isJump == true && isSlider == false){
            qDebug() << "跳跃到player线程（player）";
            qDebug() << "由于jump的休眠，暂停（player）";
            QThread::msleep(5);        //休眠50ms，等待放置数据包线程放置部分数据包
            isOK = true;
            isJump_Pause = true;
            qDebug() << "~~~~~~~~~~";
            qDebug() << "useSem" << Data_.data()->useSem.available();
            qDebug() << "unuseSem" << Data_.data()->unuseSem.available();
            Data_.data()->useSem.acquire();
            qDebug() << "++++++++++++";
            Data_.data()->unuseSem.release();

        }else if(Data_.data()->videostate->isJump == true && isSlider == true){
            qDebug() << "slider型不暂停";
            isOK = true;
            QThread::msleep(5);
            qDebug() << "isOK" << isOK;
            qDebug() << "isJumpPause" << isJump_Pause;
            Data_.data()->useSem.acquire();
            Data_.data()->unuseSem.release();
        }

        int rr = Data_.data()->packet_queue_get(Data_.data()->videostate->qpkt,packet,1);
//        qDebug() << "我可是取了数据的！！！";
        if(rr != 1){
            qDebug() << "取数据失败！";
            QThread::msleep(10);
            rr = Data_.data()->packet_queue_get(Data_.data()->videostate->qpkt,packet,1);
            if(rr != 1){
                qDebug() << "跳出来";
                break;
            }
        }

        /**视频解码**/
        Data_.data()->videostate->decode_mutex->lock();

        ret = avcodec_decode_video2(Data_.data()->videostate->pCodecCtx, Data_.data()->videostate->pFrame, &got_picture, packet);

        Data_.data()->videostate->decode_mutex->unlock();

//        qDebug() << "--------------------------";

        /**解码失败**/
        if (ret < 0)
        {
            qDebug()<<"decode error.\n";
            av_free_packet(packet);
        }
//        qDebug() << "--------------------------";
//        qDebug() << Data_.data()->videostate->isJump;
//        qDebug() << got_picture;
        //在获得图像并且当前不属于jump事件中的时候才进行格式转换
        preImage = image.copy();                            //备份一份图像，以免说出现解码失败可以使用

        if (got_picture && Data_.data()->videostate->isJump == false)
        {
            sws_scale(Data_.data()->videostate->img_convert_ctx,
                      (uint8_t const * const *) Data_.data()->videostate->pFrame->data,
                      Data_.data()->videostate->pFrame->linesize, 0, Data_.data()->videostate->pCodecCtx->height, Data_.data()->videostate->pFrameRGB->data,
                      Data_.data()->videostate->pFrameRGB->linesize);

            //把这个RGB数据 用QImage加载
            tmpImg = QImage((uchar *)Data_.data()->videostate->out_buffer_rgb,Data_.data()->videostate->pCodecCtx->width,Data_.data()->videostate->pCodecCtx->height,QImage::Format_RGB32);

            /**把图像复制一份 传递给VideoDataClass进行处理**/
            image = tmpImg.scaled(880,660);

            //计算当前时间
            //Data_.data()->videostate->decode_mutex->lock();
            Data_.data()->videostate->decode_mutex->lock();
            Data_.data()->videostate->cur_pts = packet->dts;
//            qDebug() << "cur_pts:(测试)" << packet->dts;
//            qDebug() << "cur_pts:(----)" << Data_.data()->videostate->cur_pts;
            Data_.data()->videostate->cur_time = packet->dts / Data_.data()->videostate->time_change_factor;
            Data_.data()->videostate->decode_mutex->unlock();

//            qDebug() << "--------------------------";
//            qDebug() << "isOK?" << isOK;
            if(isOK){
                qint64 seek = Data_.data()->videostate->seek_ms;
                double factor = Data_->videostate->time_change_factor;
                qint64 cur = Data_.data()->videostate->cur_pts;
                double fps = Data_.data()->videostate->fre_rate;
                //qDebug() << "-------------------------------------";
//                qDebug() << "isOK";
//                qDebug() << "factor" << factor;
//                qDebug() << "seek-ms" << seek;
//                qDebug() << "cur" << cur;
                if(seek * factor < cur ){
                    //当seek_pts和cur_pts相差过大，就是在av_seek_frame之后都相差过大，该线程休眠，进行重新搜寻
                    qDebug() << "这里要搜寻的pts是（player中搜寻结果过大）" << seek * factor;
                    qDebug() << Data_.data()->videostate->cur_pts << "但是当前的cur_pts（player）";
                    qDebug() << cur;
                    //Data_.data()->videostate->isJump = true;

                    isBadPacket = true;
                    qDebug() << "由于av_seek_frame原因解码失败，选用静帧显示";
                    continue;
                }
                qDebug() << seek * factor << "seek_pts";
                qDebug() << cur << "cur_pts";
                qDebug() << Data_.data()->videostate->cur_time << "cur_time";

                //如果不在目标帧数附近的一帧范围内
//                if(((seek * factor) < (cur - (factor * 1000 / fps) + 1))
//                    || ((seek * factor) > (cur + (factor * 1000 / fps) - 1))){
//                    //av_free_packet(packet);                                     //不在目标6000pts范围内，也许可以改成示例那样
//                    qDebug() << "抛弃";
//                    continue;
//                }
                if(seek * factor != cur){
                    qDebug() << "抛弃";
                    continue;
                }
            }
            //传送图像和时间
            emit send_CurTimeAndFrame(Data_.data()->videostate->cur_time, Data_.data()->videostate->pCodecCtx->frame_number);
            emit send_Video(image, Data_.data()->videostate->cur_time);

            if(isNor){
                QThread::msleep(fps);
            }else{
                QThread::msleep(500);
            }

            //qDebug() << "isOK" << isOK << "isJumpPause" << isJump_Pause;

            //这个是用于定位帧或者是上一帧下一帧的操作的

            if(isOK){
                Data_.data()->useSem.acquire();
                isOK = false;
                //qDebug() << "isOK值为false";
                Data_.data()->unuseSem.release();
            }
            if(isJump_Pause ){
                Data_.data()->videostate->isPause = true;
                isJump_Pause = false;
            }
        }
        av_free_packet(packet);
    }
}

//初始化
void PlayThread::init()
{
    Data_.data()->videostate->isQuit = false;
    Data_.data()->videostate->isPause = false;
}

//判断是否停止
bool PlayThread::isStop()
{
    return Data_.data()->videostate->isQuit;
}

//继续播放
void PlayThread::resume()
{
    Data_.data()->videostate->isPause = false;
    temp_mutex.lock();
    m_PauseManage.wakeAll();
    temp_mutex.unlock();
    qDebug()<< "恢复播放（player）";
}

//暂停
void PlayThread::pause()
{
    Data_.data()->videostate->isPause = true;
}

//停止播放
void PlayThread::stop()
{
    qDebug() << "停止播放";
    if(Data_.data()->videostate->isPause){
        resume();
    }
    Data_.data()->videostate->isQuit = true;
    QThread::msleep(100);
    Data_.data()->packet_queue_flush(Data_.data()->videostate->qpkt);
}

//获得下一帧
void PlayThread::getNextFrame()
{
    if(Data_.data()->videostate->isPause == true){
        resume();
    }
    qint64 cur_t = Data_.data()->videostate->cur_time;
    qint64 NextF = 1000 / Data_.data()->videostate->fre_rate;
    Data_.data()->videostate->seek_ms = cur_t + NextF;
    qint64 max = Data_.data()->videostate->pFormatCtx->duration / 1000;
    if(Data_.data()->videostate->seek_ms > max){
        QMessageBox::information(nullptr, "", QStringLiteral("搜寻的帧超出范围（过大）"));
        return;
    }
    qDebug() << "这里要搜寻的frame是（player中的getNextFrame）" << Data_.data()->videostate->seek_ms;
    qDebug() << "当前pts：" << Data_->videostate->cur_pts;
    Data_.data()->videostate->isJump = true;
    isSlider = false;
}

//获得前一帧
void PlayThread::getPreFrame()
{
    if(Data_.data()->videostate->isPause == true){
        resume();
    }
    qint64 cur_t = Data_.data()->videostate->cur_time;
    qint64 preF = 1000 / Data_.data()->videostate->fre_rate;
    Data_.data()->videostate->seek_ms = cur_t - preF;
    if(Data_.data()->videostate->seek_ms < 0){
        QMessageBox::information(nullptr,"","搜寻的帧超出范围（过小）");
        return;
    }
    qDebug() << "这里要搜寻的frame是（player中的getPreFrame）" << Data_.data()->videostate->seek_ms;
    qDebug() << "当前pts：" << Data_->videostate->cur_pts;
    Data_.data()->videostate->isJump = true;
    isSlider = false;
}

//改变位置
void PlayThread::changePos(int pos)
{
    if(Data_.data()->videostate->isPause == true){
        resume();
    }
    qDebug() << "有没有changePos";
    double tmp = pos / 200.0;
    qint64 time = Data_.data()->videostate->pFormatCtx->duration / 1000;
    qDebug() << tmp << time;
    isSlider = true;                                        //如果是slider拖动放置的话就不进行暂停
    Data_.data()->videostate->isJump = true;
    Data_.data()->videostate->seek_ms = time * tmp;
}

//跳到三十帧后的位置
void PlayThread::gotoNext1sec()
{
    if(Data_.data()->videostate->isPause == true){
        resume();
    }
    qint64 cur = Data_.data()->videostate->cur_time;
    qint64 next30f = (1000 / Data_.data()->videostate->fre_rate) * 30;
    qint64 next1Sec = cur + next30f;
    Data_.data()->videostate->seek_ms = next1Sec;
    qint64 max = Data_.data()->videostate->pFormatCtx->duration / 1000;
    if(Data_.data()->videostate->seek_ms > max){
        QMessageBox::information(nullptr, "", QStringLiteral("搜寻的帧超出范围（过大）"));
        return;
    }
    Data_.data()->videostate->isJump = true;
}

//实际是之前的跳到三十帧之前的位置
void PlayThread::gotoPre1sec()
{
    if(Data_.data()->videostate->isPause == true){
        resume();
    }
    qint64 cur = Data_.data()->videostate->cur_time;
    qint64 p1s = (1000 / Data_.data()->videostate->fre_rate) * 30;
    qint64 pre1Sec = cur - p1s;
    Data_.data()->videostate->seek_ms = pre1Sec;
    if(Data_.data()->videostate->seek_ms < 0){
        QMessageBox::information(nullptr,"","搜寻的帧超出范围（过小）");
        return;
    }
    Data_.data()->videostate->isJump = true;
    //isSlider = true;
}

//测试接口--弃用
void PlayThread::gotoTest()
{
    if(Data_.data()->videostate->isPause == true){
        resume();
    }
    Data_.data()->videostate->isJump = true;
    Data_.data()->videostate->seek_ms = 115000;
}

//跳动某位置
void PlayThread::gotoPos(int frame, int fps)
{
    if(Data_.data()->videostate->isPause == true){
        resume();
    }
    qint64 pos = frame * (1000 / fps);

    Data_.data()->videostate->seek_ms = pos;

    //判断是否超过选取范围
    qint64 max = Data_.data()->videostate->pFormatCtx->duration / 1000;
    if(Data_.data()->videostate->seek_ms > max){
        QMessageBox::information(nullptr, "", QStringLiteral("搜寻的帧超出范围（过大）"));
        return;
    }else if(Data_.data()->videostate->seek_ms < 0){
        QMessageBox::information(nullptr,"","搜寻的帧超出范围（过小）");
        return;
    }

    //跳跃
    qDebug() << "这里要搜寻的ms是（player中的gotoPos）" << Data_.data()->videostate->seek_ms;
    isSlider = false;
    Data_.data()->videostate->isJump = true;
}

//常速播放
void PlayThread::normalVel()
{
    isNor = true;
}

//慢速播放
void PlayThread::slowVel()
{
    isNor = false;
}


