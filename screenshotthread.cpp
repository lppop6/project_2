#include "screenshotthread.h"

//static int angulartimes = 0;                                    //作为标记角点范围的记录次数angulartimes = 0;

ScreenShotThread::ScreenShotThread(QObject *parent) : QObject(parent)
{
    qDebug() << "ScreenShotThread构造";

}

ScreenShotThread::~ScreenShotThread()
{
    qDebug() << "ScreenShotThread析构";
}

//style: 1、截屏  2、选取角点区域  3、色差范围
void ScreenShotThread::markQImage()
{
    QImage background = imageprocessing;             //深拷贝一个适合于该窗口的画布
    QPainter p;
    p.begin(&background);
    p.setPen(Qt::red);

    if(style == 1){
        p.drawRect(pre_x, pre_y, cur_x - pre_x, cur_y - pre_y);
        p.setPen(Qt::gray);
        p.drawLine(pre_x ,pre_y, cur_x, cur_y);
        p.drawLine(cur_x, pre_y, pre_x, cur_y);
    }else if(style == 2){
        p.drawRect(pre_x, pre_y, cur_x - pre_x, cur_y - pre_y);
        //p.drawLine(pre_x ,pre_y, cur_x, cur_y);
    }else if(style == 3){
        p.drawRect(pre_x, pre_y, cur_x - pre_x, cur_y - pre_y);
        double distance_x = (cur_x - pre_x) / 38.0;
        //qDebug() << "distance_X" << distance_x;
        double distance_y = (cur_y - pre_y) / 26.0;
        //qDebug() << "dis_Y" << distance_y;
        double x = pre_x + 2 * distance_x;
        double y = pre_y + 2 * distance_y;
        double w = distance_x * 4;
        double h = distance_y * 4;
        for(int i = 0; i < 4; ++i){
            for(int j = 0; j < 6; ++j){
                p.drawRect(x,y,w,h);
                x = x + distance_x * 6;
            }
            y = y + distance_y * 6;
            x = pre_x + 2 * distance_x;
        }
    }else{
        return;
    }
    p.end();
    emit sendMarkedPic(background);
    if(background.isNull()){

    }else{

    }
}

void ScreenShotThread::getDrawStyle(int style_)
{
    style = style_;
}

//在点击鼠标右键时触发的事件
void ScreenShotThread::FullColor(QImage background ,int prex, int curx, int prey, int cury)
{
    QColor mycolor(Qt::gray);
    mycolor.setAlpha(100);
    QPainter p;
    p.begin(&background);
    p.setBrush(mycolor);
    p.drawRect(prex, prey, curx - prex, cury - prey);
    p.end();
    emit sendMarkedPic(background);
    qDebug() << "确定有进入！！！";
}



void ScreenShotThread::getPointAndQImage(QImage temp, int prex, int curx, int prey, int cury)
{
    imageprocessing = temp;
    pre_x = prex;
    pre_y = prey;
    cur_y = cury;
    cur_x = curx;
    markQImage();
}
