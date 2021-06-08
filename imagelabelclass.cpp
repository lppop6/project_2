#include "imagelabelclass.h"

static int angulartimes = 0;

/*
 * 进行辅助选取角点范围的时候，放大缩小和平移操作时不允许的。
 */
ImageLabelClass::ImageLabelClass(QWidget *parent) : QLabel(parent)
{
    qDebug() << "ImageLabel构造";
    setMouseTracking(true);                     //必须要打开鼠标追踪，才能显示当前鼠标位置
    multiple = 1;                               //初始放大倍数
    prepositionx = 0;                           //用于移动图片
    prepositiony = 0;
    curpositionx = 0;
    curpositiony = 0;

    specialstyle = 0;                           //初始的特殊模式

    isleftpress = false;                        //将特殊模式中的判断模式标志符置位为false
    angularmode = false;
    cutchromaticmode = false;
    cutpicturemode = false;
    isendmarked = false;
    isendmarked_1 = false;
    //lock = false;

    screenshotthread = new ScreenShotThread;    //启动线程
    shotthread = new QThread(this);
    screenshotthread->moveToThread(shotthread);
    shotthread->start();

    connect(this,&ImageLabelClass::sendSpecialModeDraw,screenshotthread,&ScreenShotThread::getPointAndQImage);
    connect(screenshotthread,&ScreenShotThread::sendMarkedPic,this,&ImageLabelClass::getMarkedImageFromThread);
    connect(this, &ImageLabelClass::destroyed, this, &ImageLabelClass::dealThreadClose);
    connect(this,&ImageLabelClass::sendSpecialModeStyle,screenshotthread,&ScreenShotThread::getDrawStyle);
    connect(this, &ImageLabelClass::sendFullColor,screenshotthread,&ScreenShotThread::FullColor);


    QPainter p ;

    p.begin(&ss);
    p.setBrush(Qt::lightGray);
    p.setPen(Qt::white);
    p.drawRect(0,0,3000,3000);
    p.end();
}

ImageLabelClass::~ImageLabelClass()
{
    qDebug() << "ImageLabel析构";
}

//滚轮事件，用于ctrl + 滚轮 进行放大缩小
void ImageLabelClass::wheelEvent(QWheelEvent *event)
{

    //qDebug() << "滚轮事件";
    if(QApplication::keyboardModifiers() == Qt::ControlModifier){
        if(event->delta() > 0 && angularmode == false && cutchromaticmode == false){                                     //在进行角点范围选取的时候，不允许放大缩小操作
            //别问我为什么减2，因为我也不明白！！！！！但是不减2就会造成窗口不断变大
            QImage background = ss.copy(0, 0, this->width(), this->height() - 2);       //深拷贝一个适合于该窗口的画布
            QPainter p;
            p.begin(&background);
            //qDebug() << "放大";
            QImage temp;                                                    //其实可以不用temp作为载体，直接用imageprocessed就行了

            multiple = 1.25;                                                //每次放大倍数为1.25倍
            imagewidth = imagewidth * multiple;
            imageheight = imageheight * multiple;
            //对imageprocessed进行放大，但是注意这里放大之后，imageprocessed自身应该也是放大了的
            temp = imageprocessed.scaled(imagewidth, imageheight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            double pointx = 0;
            double pointy = 0;                                              //为最终的放置图片的顶点坐标

            pointx = nextx2 - (positionx - nextx2) * (multiple - 1);
            pointy = nexty2 - (positiony - nexty2) * (multiple - 1);        //计算以鼠标点为中心的放大倍数

            nextx2 = pointx;
            nexty2 = pointy;                                                //更新顶点坐标，用于下次放大缩小的操作

            qDebug() << "Temp size :" << temp.size();

            qDebug() << "nextx2:   " << nextx2 << "nexty2:     " << nexty2;
            qDebug() << "positionx:   " << positionx << "positiony:  " << positiony;
            p.drawImage(pointx,pointy,temp);                                //在画布上放置temp图片
            p.end();

            tempcopy = temp;                                                //更新tempcopy，为后续的移动平移提供原图像（平移之后图像大小是不能变的）
            drawcopy = background;                                          //更新drawcopy，为后续的特殊模式标记提供背景
            angularcopy = background;                                       //更新angularcopy，为后续的角点选择提供背景
            angulartempcopy = background;
            emit send_ZoomOrTransfer(background);                           //发送放大后的图片给MainWindow
            qDebug() << "background size(): " << background.size();

        }else if(event->delta() <= 0 && angularmode == false && cutchromaticmode == false){
            QImage background = ss.copy(0 ,0 ,this->width() ,this->height() - 2);

            QPainter p;
            p.begin(&background);
            QImage temp;
            multiple = 0.8;

            imagewidth = imagewidth * multiple;
            imageheight = imageheight * multiple;

            temp = imageprocessed.scaled(imagewidth, imageheight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            double pointx = 0;                                              //为最终的放置图片的顶点坐标
            double pointy = 0;

            pointx = nextx2 - (positionx - nextx2) * (multiple - 1);
            pointy = nexty2 - (positiony - nexty2) * (multiple - 1);        //放置图片的位置的计算公式

            nextx2 = pointx;
            nexty2 = pointy;                                                //更新下次图像的顶点

            p.drawImage(pointx,pointy,temp);
            p.end();
            tempcopy = temp;                                               //更新tempcopy，为后续的移动平移提供原图像（平移之后图像大小是不能变的）
            drawcopy = background;                                         //更新drawcopy，为后续的特殊模式标记提供背景
            emit send_ZoomOrTransfer(background);                          //发送缩放的图片给MainWindow
        }
    }
}

//鼠标移动事件，主要用来获得鼠标位置
void ImageLabelClass::mouseMoveEvent(QMouseEvent *ev)
{
    positionx = ev->x();
    positiony = ev->y();                                                        //这两个参数时用来记录当前鼠标位置
    if((angularmode || cutchromaticmode || cutpicturemode) && isleftpress){
        curpositionx_1 = ev->x();
        curpositiony_1 = ev->y();                                               //记录在特殊模式下移动时的坐标
        //emit sendSpecialModeStyle(specialstyle);
        if(angularmode){
            emit sendSpecialModeDraw(angulartempcopy,prepositionx_1, curpositionx_1, prepositiony_1, curpositiony_1);
        }else{
            emit sendSpecialModeDraw(drawcopy,prepositionx_1, curpositionx_1, prepositiony_1, curpositiony_1);
        }

        //emit sendSpecialModeDraw(drawcopy,prepositionx_1, curpositionx_1, prepositiony_1, curpositiony_1);
    }
}

//鼠标按压事件
void ImageLabelClass::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::MidButton){
        this->setCursor(Qt::ClosedHandCursor);                                  //变化为手型鼠标
        prepositionx = ev->x();
        prepositiony = ev->y();                                                 //按压鼠标左键的时候获取此时坐标并记录
        qDebug() << "按压  x:  " << prepositionx << "y:  " << prepositiony;
    }
    if(ev->button() == Qt::LeftButton && (angularmode || cutchromaticmode || cutpicturemode)){
        isleftpress = true;
        prepositionx_1 = ev->x();
        prepositiony_1 = ev->y();                                                //记录下在特殊模式按压左键时的坐标
        //        qDebug() << "x_pre:  " << prepositionx_1 << "y_pre:  " << prepositiony_1;
        //        qDebug() << "x_cur:  " << curpositionx_1 << "y_cur:  " << curpositiony_1;
    }
    if(ev->button() == Qt::RightButton && angularmode  && isendmarked){
        isendmarked = false;
        isendmarked_1 = true;
        ++angulartimes;
        emit sendFullColor(angularcopy,prepositionx_1,curpositionx_1,prepositiony_1,curpositiony_1);


        //确保无论范围是怎么样选择的，每个分割的小块左上方顶点坐标必定在右下方顶点坐标之前
        if(prepositionx_1 > curpositionx_1){
            int swaptemp = 0;
            swaptemp = prepositionx_1;
            prepositionx_1 = curpositionx_1;
            curpositionx_1 = swaptemp;
        }
        if(prepositiony_1 > curpositiony_1){
            int swaptemp = 0;
            swaptemp = prepositiony_1;
            prepositiony_1 = curpositiony_1;
            curpositiony_1 = swaptemp;
        }

        //从左上方开始，按顺时针把四个小块的范围放进angularvector
        angularvector.push_back(prepositionx_1);
        angularvector.push_back(curpositionx_1);
        angularvector.push_back(prepositiony_1);
        angularvector.push_back(curpositiony_1);
    }
}

//鼠标释放事件
void ImageLabelClass::mouseReleaseEvent(QMouseEvent *ev)
{
    this->setCursor(Qt::ArrowCursor);                                       //箭头型鼠标
    if(ev->button() == Qt::MidButton && angularmode == false && cutchromaticmode == false){              //在进行角点范围选取的时候，不允许平移
        curpositionx = ev->x();
        curpositiony = ev->y();                                             //释放鼠标左键的时候获取此时坐标并记录
        //移动图片
        QImage background = ss.copy(0, 0, this->width(), this->height() - 2);             //深拷贝一个适合于该窗口的画布
        QPainter p;
        p.begin(&background);
        nextx2 = curpositionx - prepositionx + nextx2;
        nexty2 = curpositiony - prepositiony + nexty2;                      //计算图片左上角顶点的坐标，并记录下来
        p.drawImage(nextx2, nexty2, tempcopy);
        p.end();
        drawcopy = background;                                              //更新drawcopy，为后续的特殊模式的标记提供背景
        emit send_ZoomOrTransfer(background);
    }

    if(ev->button() == Qt::LeftButton && (angularmode || cutchromaticmode || cutpicturemode)){

        curpositionx_1 = ev->x();
        curpositiony_1 = ev->y();                                           //记录在特殊模式下释放左键时的坐标
        isleftpress = false;                                                //鼠标左键释放之后移动鼠标就不再绘图了
        //emit sendSpecialModeStyle(specialstyle);
        if(angularmode && !isendmarked){
            if(angulartimes == 4){
                angularcopy = drawcopy;
                angulartempcopy = drawcopy;
                angulartimes = 0;
            }
            isendmarked = true;

            emit sendSpecialModeDraw(angularcopy,prepositionx_1, curpositionx_1, prepositiony_1, curpositiony_1);
            angulartempcopy = angularcopy.copy(0,0,angularcopy.width(),angularcopy.height());
            qDebug() << "此时是：   " <<angulartimes;
        }else if(cutchromaticmode){
            //chrovector也是按照先pre后cur ，先x后y来计算
            emit sendSpecialModeDraw(drawcopy,prepositionx_1, curpositionx_1, prepositiony_1, curpositiony_1);
            //chrovector.push_back(prepositionx_1);
            //chrovector.push_back(curpositionx_1);
            //chrovector.push_back(prepositiony_1);
            //chrovector.push_back(curpositiony_1);
            chrovector.push_front(curpositiony_1);
            chrovector.push_front(prepositiony_1);
            chrovector.push_front(curpositionx_1);
            chrovector.push_front(prepositionx_1);
        }else{
            emit sendSpecialModeDraw(drawcopy,prepositionx_1, curpositionx_1, prepositiony_1, curpositiony_1);
        }
    }
}

//label大小重置--弃用
void ImageLabelClass::resizeEvent(QResizeEvent *event)
{
    qDebug() << "窗口变换事件！！";
}

//再次初始化，用于窗口变化时，重置位置参数
void ImageLabelClass::reInitImagelabel()
{
    multiple = 1;                               //初始放大倍数
    prepositionx = 0;                           //用于移动图片
    prepositiony = 0;
    curpositionx = 0;
    curpositiony = 0;

    isleftpress = false;                        //将特殊模式中的判断模式标志符置位为false
    angularmode = false;
    cutchromaticmode = false;
    cutpicturemode = false;
}

//导入新图片进行的操作
void ImageLabelClass::firstPaintOnQImage(QImage temp)
{
    qDebug() << "把图片放在正中心！！";
    QPainter p;
    QImage background = ss.copy(0,0,this->width(),this->height());
    p.begin(&background);
    qDebug() << this->width() << "w";
    qDebug() << this->height() << "h";
    int x = this->width() / 2 - imageprocessed.width() / 2;
    int y = this->height() / 2 - imageprocessed.height() / 2;

    qDebug() << "x:" << x << "y:" << y;

    p.drawImage(x, y, temp);
    p.end();
    drawcopy = background;//.copy(0,0,background->width(),background->height());
    //this->setPixmap(QPixmap::fromImage(background));
    emit send_ZoomOrTransfer(background);
}

//开始检测几畸变
double ImageLabelClass::startTesting_Dis()
{
    if(angularvector.size() < 16){
        qDebug() << "未选取所有点！！！";
        qDebug() << angularvector.size();
        return -1;
    }
    QScopedPointer<ImageProcessingClass> imagepro(new ImageProcessingClass);             //实例化图像处理的类

    //QImage temp = drawcopy.copy(50,50,drawcopy.width(),drawcopy.height());
    double result = imagepro.data()->getQImageFromLabel_Dis(drawcopy,angularvector);
    //emit sendReslt_Dis(result);
    angularvector.clear();
    return result;
}

//开始检测色差
QVector<int> ImageLabelClass::startTesting_Chro()
{
    qDebug() << "????色差";
    QVector<int> result;
    if(chrovector.size() < 4){
        qDebug() << "未进行色差选取！！！";
        qDebug() << chrovector.size();
        return result;
    }
    QScopedPointer<ImageProcessingClass> imagepro(new ImageProcessingClass);
    result = imagepro.data()->getQImageFromLabel_Chro(drawcopy,chrovector);
    //emit sendResult_Chro(result);
    chrovector.clear();
    return result;
}

//重置重新选择顶点的参数
void ImageLabelClass::reChoosesAngular()
{
    qDebug() << angulartimes;
    angularvector.clear();
    angulartimes = 0;
    qDebug() << angulartimes;
    angularcopy = drawcopy;
    //chrovector.clear();
}


//void ImageLabelClass::chromaticComplete()
//{
//    QImage temp = drawcopy;


//}

//槽函数，获取MainWindow中发来的图像，就是导入的图像
void ImageLabelClass::get_Image(QImage temp)
{
    //reInitImagelabel();
    imageprocessed = temp.copy(0, 0, temp.width(), temp.height());                  //进行深拷贝
    tempcopy = imageprocessed;
    //drawcopy = imageprocessed;
    imagewidth = imageprocessed.width();
    imageheight = imageprocessed.height();                                          //获取该图像的长宽
    nextx2 = (this->width() - imagewidth) / 2;                                      //由于我们初始放置的时候，图像都是居中放置的，位于label的中心
    nexty2 = (this->height() - imageheight) / 2;                                    //nextx2,nexty2是指该次图像放置的左上顶点位置，用于判断下次放置位置
}

//进行扫描，然后选取鼠标附近的角点，点击确认角点位置
//调用opencv进行二值化处理？还是怎么样呢？
//可以在点击事件中增加一个shift + 左键，依次选取角点可能的范围，当然鼠标移动到哪个位置哪个位置的附近位置就会有搜索角点范围
void ImageLabelClass::Choose_Angular_Point()
{
    angularmode = true;
    emit sendSpecialModeStyle(1);               //1是thread里面选取顶点范围的参数
    angularcopy = drawcopy;                     //备份背景图。
    angulartempcopy = angularcopy.copy(0,0,angularcopy.width(),angularcopy.height());
}

//截屏的切割图像
void ImageLabelClass::cutPicture()
{
    cutpicturemode = true;
    emit sendSpecialModeStyle(2);               //2是thread里面选取截图的参数
    prepositionx_1 = 0;
    prepositiony_1 = 0;
    curpositionx_1 = drawcopy.width();
    curpositiony_1 = drawcopy.height();
}

//选择色差检测的范围，就顶点值
void ImageLabelClass::chooseChromatic()
{
    cutchromaticmode = true;
    emit sendSpecialModeStyle(3);               //3是thread里面选取色块的参数
}

//用于角点选择时点击右键后角点选择区域变成灰色透明
void ImageLabelClass::getMarkedImageFromThread(QImage temp)
{
    //只有在点击右键确认角点范围之后，才把这次的背景图保存，用于下一个选取范围
    if(angularmode && isendmarked_1){
        angularcopy = temp.copy(0,0,temp.width(),temp.height());
        angulartempcopy = angularcopy.copy(0,0,angularcopy.width(),angularcopy.height());
        //angularcopy = temp;
        //qDebug() << "angular!!!!!复制了!!!!";
        isendmarked_1 = false;
    }
    emit send_ZoomOrTransfer(temp);             //从thread中获取标记的图像，并发送到mainwindow中显示
    //其实也可以用this->setPixmap显示吧。。
}

//退出线程，在窗口关闭时退出
void ImageLabelClass::dealThreadClose()
{
    shotthread->quit();
    shotthread->wait();
    delete screenshotthread;
}

//退出特殊模式
void ImageLabelClass::quitSpecialMode()
{
    angularmode = false;
    cutpicturemode = false;
    cutchromaticmode = false;
    emit sendSpecialModeStyle(0);
    emit send_ZoomOrTransfer(drawcopy);
}

//完成屏幕截取接口并显示
void ImageLabelClass::completeScreenShot()
{
    QImage result = drawcopy.copy(prepositionx_1, prepositiony_1, curpositionx_1 - prepositionx_1,  curpositiony_1 - prepositiony_1);
    //emit send_ZoomOrTransfer(result);
    get_Image(result);
    firstPaintOnQImage(result);
}


