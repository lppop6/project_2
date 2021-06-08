#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置初始的ui界面
    ui->splitter_1->setStretchFactor(0, 2);
    ui->splitter_1->setStretchFactor(1, 10);
    ui->splitter_2->setStretchFactor(0, 2);
    ui->splitter_2->setStretchFactor(1, 6);
    ui->splitter_3->setStretchFactor(0, 26);
    ui->splitter_3->setStretchFactor(1, 1);

    ui->splitter_5->setStretchFactor(0, 2);
    ui->splitter_5->setStretchFactor(1, 10);
    ui->splitter_6->setStretchFactor(0, 2);
    ui->splitter_6->setStretchFactor(1, 6);
    ui->splitter_4->setStretchFactor(0, 26);
    ui->splitter_4->setStretchFactor(1, 1);

    //初始化图像和视频参数的值，用于判断是否进行检测，要是未进行检测的话，这个值应该会不可能取到
    grayScale = -1000;
    distort = -1000;
    resolution = -1000;
    chromatic = -1000;
    codeRate_r = -1000;
    frameFre_r = -1000;
    delayresu = INT_MIN;
    ispassSub = false;

    //定义下frameFre = 0配合后面代码保证在ff线程获得了fps之后在进入playe线程进行解码
    frameFre = 0;
    //初始按钮变灰
    ui->btn_ScreenShotComplete->setDisabled(true);
    ui->btn_AngularPointComplete->setDisabled(true);
    ui->btn_ChromaticComplete->setDisabled(true);
    ui->btn_ReChooseAngular->setDisabled(true);
    ui->btn_ChooseChromatic->setDisabled(true);

    //QSlider的设置
    ui->sld_ProgressBar->setMaximum(200);
    ui->sld_ProgressBar->setMinimum(0);
    ui->sld_ProgressBar->setSingleStep(1);
    ui->sld_ProgressBar->setTracking(true);

    //设置初始化lbl_Image、lbl_Video为居中显示
    ui->lbl_Image->setAlignment(Qt::AlignCenter);
    ui->lbl_Video->setAlignment(Qt::AlignCenter);
    //设置初始化rbn_Distortion为选中状态
    ui->rbn_Distortion->setChecked(true);
    //设置初始化rbn_FrameFreq为选中状态
    ui->rbn_FrameFreq->setChecked(true);
    //用于平移放大缩小图像的信号和槽
    connect(ui->lbl_Image, &ImageLabelClass::send_ZoomOrTransfer, this, &MainWindow::image_Show);
    connect(this,&MainWindow::send_QImage_to_ImageLabel, ui->lbl_Image, &ImageLabelClass::get_Image);
    connect(this,&MainWindow::send_AngularPoint, ui->lbl_Image, &ImageLabelClass::Choose_Angular_Point);
    connect(this,&MainWindow::send_import_Image,ui->lbl_Image, &ImageLabelClass::firstPaintOnQImage);

    //用于退出特殊模式
    connect(this,&MainWindow::send_QuitMode,ui->lbl_Image,&ImageLabelClass::quitSpecialMode);
    connect(this,&MainWindow::send_ScreenShot,ui->lbl_Image,&ImageLabelClass::cutPicture);
    connect(this,&MainWindow::send_Chromatic,ui->lbl_Image,&ImageLabelClass::chooseChromatic);
    connect(this,&MainWindow::send_SceeenShotComplete,ui->lbl_Image,&ImageLabelClass::completeScreenShot);
    connect(this,&MainWindow::send_ReChoseAngular,ui->lbl_Image,&ImageLabelClass::reChoosesAngular);


    ui->lbl_Image->resize(ui->groupBox_2->size());              //重新调节下尺寸
    this->showMaximized();
    //记录下lbl_Image的值，用于调节显示的图像大小
    labelheight = ui->lbl_Image->height();
    labelwidth = ui->lbl_Image->width();

    isdealclose = false;                        //用于再次导入视频或是直接退出软件时的资源释放
    isFirstChro = true;                         //用于判断是否为第一次色差比对
    //isOver = false;                             //导入多次视频就会出现重复析构，重复调用dealclosse

    //初始化视频按钮变灰
    ui->btn_goto_next_5s->setDisabled(true);
    ui->btn_goto_pre_5s->setDisabled(true);
    ui->btn_1X->setDisabled(true);
    ui->btn_2X->setDisabled(true);
    ui->btn_Locate->setDisabled(true);
    ui->btn_NextFrame->setDisabled(true);
    ui->btn_pauseVideo->setDisabled(true);
    ui->btn_StartTestVideo->setDisabled(true);
    ui->btn_PreviousFrame->setDisabled(true);
    ui->btn_resumeVideo->setDisabled(true);
    ui->btn_stopVideo->setDisabled(true);


}

MainWindow::~MainWindow()
{
    delete ui;
}

//显示图像
void MainWindow::image_Show(QImage temp)
{
    ui->lbl_Image->setPixmap(QPixmap::fromImage(temp));
}

//窗口变换事件，窗口变换之后那个图片也要跟着变换。
void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->lbl_Image->resize(ui->groupBox_2->size());                            //这个很重要，详细见https://blog.csdn.net/qq_42100881/article/details/80449150
    emit send_QImage_to_ImageLabel(orgin);
    ui->lbl_Image->setPixmap(QPixmap::fromImage(orgin));
}

//用于二次提交表格的时候进行的excel重置
void MainWindow::reSetExcel()
{
    ispassSub = false;
    isdisturb = false;
    isfluency = false;
    ismasic = false;
    isend = false;
    testerN = "";
    testingN = "";
    modelN = "";
    grayScale = -1000;
    distort = -1000;
    resolution = -1000;
    chromatic = -1000;
    codeRate = -1000;
    frameFre_r = -1000;
    delayresu = INT_MIN;
    ispassSub = false;

}

//提交表格之后重置预览检测结果
void MainWindow::reSetLwtForm()
{
    ui->lwt_Form_1->takeItem(2);
    ui->lwt_Form_2->takeItem(2);
    ui->lwt_Form_1->insertItem(2,tr("色差检测结果："));
    ui->lwt_Form_2->insertItem(2,tr("色差检测结果："));

    ui->lwt_Form_1->takeItem(3);
    ui->lwt_Form_2->takeItem(3);
    ui->lwt_Form_1->insertItem(3,tr("几何失真结果："));
    ui->lwt_Form_2->insertItem(3,tr("几何失真结果："));

    ui->lwt_Form_1->takeItem(1);
    ui->lwt_Form_2->takeItem(1);
    ui->lwt_Form_1->insertItem(1,tr("分辨率值："));
    ui->lwt_Form_2->insertItem(1,tr("分辨率值："));

    ui->lwt_Form_1->takeItem(4);
    ui->lwt_Form_2->takeItem(4);
    ui->lwt_Form_1->insertItem(4,tr("灰阶值："));
    ui->lwt_Form_2->insertItem(4,tr("灰阶值："));

    ui->lwt_Form_1->takeItem(6);
    ui->lwt_Form_2->takeItem(6);
    ui->lwt_Form_1->insertItem(6,tr("帧频值："));
    ui->lwt_Form_2->insertItem(6,tr("帧频值："));

    ui->lwt_Form_1->takeItem(7);
    ui->lwt_Form_2->takeItem(7);
    ui->lwt_Form_1->insertItem(7,tr("码率值："));
    ui->lwt_Form_2->insertItem(7,tr("码率值："));

    ui->lwt_Form_1->takeItem(8);
    ui->lwt_Form_2->takeItem(8);
    ui->lwt_Form_1->insertItem(8,tr("延时值："));
    ui->lwt_Form_2->insertItem(8,tr("延时值："));

    ui->lwt_Form_1->takeItem(9);
    ui->lwt_Form_2->takeItem(9);
    ui->lwt_Form_1->insertItem(9,tr("干扰："));
    ui->lwt_Form_2->insertItem(9,tr("干扰："));

    ui->lwt_Form_1->takeItem(10);
    ui->lwt_Form_2->takeItem(10);
    ui->lwt_Form_1->insertItem(10,tr("马赛克现象："));
    ui->lwt_Form_2->insertItem(10,tr("马赛克现象："));

    ui->lwt_Form_1->takeItem(11);
    ui->lwt_Form_2->takeItem(11);
    ui->lwt_Form_1->insertItem(11,tr("流畅性不足："));
    ui->lwt_Form_2->insertItem(11,tr("流畅性不足："));

    ui->lwt_Form_1->takeItem(12);
    ui->lwt_Form_2->takeItem(12);
    ui->lwt_Form_1->insertItem(9,tr("抖动："));
    ui->lwt_Form_2->insertItem(9,tr("抖动："));

}

//二次导入视频的重置
void MainWindow::reSetVideo()
{

}

//用于导入图片并发送信号进行显示
void MainWindow::on_btn_ImportImage_clicked()
{
    QString imagefilepath = QFileDialog::getOpenFileName(
        this, "选择要测试的图片",
        "./",
        "图片文件 (*.png *.jpg *.tiff *.bmp);; 所有文件 (*.*);; ");

    if(imagefilepath.isNull()){
        return;
    }

    testimage = QImage(imagefilepath);
    emit send_QImage_to_ImageLabel(testimage);                              //发送待测试图片给ImageLabel类，在那个类里面进行缩放。
    emit send_import_Image(testimage);
    qDebug() << "orgin的size:" << testimage.size();
    orgin = testimage.copy(0,0,testimage.width(),testimage.height());
}

//用于选择角点
void MainWindow::on_btn_ChooseAngularPoint_clicked()
{
    ui->btn_ReChooseAngular->setEnabled(true);
    if(ui->lbl_Image->pixmap()->isNull()){
        qDebug() << "此时无图像！！！！！！！！！！！！！！！！！！请先导入图像";
        return;
    }
    if(ui->rbn_Distortion->isChecked() == true){
        emit send_AngularPoint();                                           //如果选择选取角点的选项的话，就向ImageLabelClass发送一个获取角点的信号
        ui->btn_AngularPointComplete->setEnabled(true);
        ui->btn_ChooseAngularPoint->setDisabled(true);
    }
}

//用于角点选择完成后退出特殊模式
void MainWindow::on_btn_AngularPointComplete_clicked()
{
    emit send_QuitMode();
    ui->btn_AngularPointComplete->setDisabled(true);
    ui->btn_ChooseAngularPoint->setEnabled(true);
}

//就是把副本作为新的文件导进去
void MainWindow::on_btn_RemapImage_clicked()
{
    emit send_QImage_to_ImageLabel(orgin);
    emit send_import_Image(orgin);
    ui->btn_AngularPointComplete->setDisabled(true);
    ui->btn_ChooseAngularPoint->setEnabled(true);
    ui->btn_ChooseChromatic->setEnabled(true);
    ui->btn_ChromaticComplete->setDisabled(true);
    ui->btn_ScreenShot->setEnabled(true);
    ui->btn_ScreenShotComplete->setDisabled(true);
    ui->btn_ReChooseAngular->setDisabled(true);
    //image_Show(orgin);
}

//用于截屏
void MainWindow::on_btn_ScreenShot_clicked()
{
    if(ui->lbl_Image->pixmap()->isNull()){
        qDebug() << "无图像";
        return;
    }
    emit send_ScreenShot();
    ui->btn_ScreenShotComplete->setEnabled(true);
    ui->btn_ScreenShot->setDisabled(true);
}

//截屏完成
void MainWindow::on_btn_ScreenShotComplete_clicked()
{
    emit send_SceeenShotComplete();
    emit send_QuitMode();
    ui->btn_ScreenShotComplete->setDisabled(true);
    ui->btn_ScreenShot->setEnabled(true);
}

//选择色差点
void MainWindow::on_btn_ChooseChromatic_clicked()
{
    if(ui->lbl_Image->pixmap()->isNull()){
        qDebug() << "无图像";
        return;
    }
    emit send_Chromatic();
    ui->btn_ChooseChromatic->setDisabled(true);
    ui->btn_ChromaticComplete->setEnabled(true);
    //ui->btn_ReChooseAngular->setEnabled(true);
}

//色差模式退出--分为两次
void MainWindow::on_btn_ChromaticComplete_clicked()
{
    if(isFirstChro){
        qDebug() << "进行第一次色差检测";
        Chro_1 = ui->lbl_Image->startTesting_Chro();
        QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("请导入第二张图片进行色差比对获得检测结果"));
        ui->btn_StartTestImage->setDisabled(true);
        isFirstChro = false;
    }else{
        qDebug() << "进行第二次色差检测";
        Chro_2 = ui->lbl_Image->startTesting_Chro();
        QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("现在可以进行色差检测"));
        ui->btn_StartTestImage->setEnabled(true);
    }
    emit send_QuitMode();
    ui->btn_ChooseChromatic->setEnabled(true);
    ui->btn_ChromaticComplete->setDisabled(true);
    ui->btn_ReChooseAngular->setDisabled(true);
}

//开始测试成像性能
void MainWindow::on_btn_StartTestImage_clicked()
{
    if(ui->rbn_Distortion->isChecked()){
        distort = ui->lbl_Image->startTesting_Dis() * 100;
        qDebug() << "畸变分析结果：   " << distort;
        QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("该图像的畸变率为 %1 %").arg(distort));
        ui->lwt_Form_1->takeItem(3);
        ui->lwt_Form_2->takeItem(3);
        ui->lwt_Form_1->insertItem(3,tr("几何失真结果： \t %1 %").arg(distort));
        ui->lwt_Form_2->insertItem(3,tr("几何失真结果： \t %1 %").arg(distort));
    }
    if(ui->rbn_Chromatic->isChecked()){
        isFirstChro = true;                                     //重置
        chromatic = getChroResult(Chro_1,Chro_2);
        qDebug() << "Chro_1";
        for(auto i : Chro_1){
            qDebug() << i;
        }
        qDebug() << "Chro_2";
        for(auto i : Chro_2){
            qDebug() << i;
        }
        QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("该图像的总色差为 %1").arg(chromatic));
        ui->lwt_Form_1->takeItem(2);
        ui->lwt_Form_2->takeItem(2);
        ui->lwt_Form_1->insertItem(2,tr("色差检测结果： \t %1 %").arg(chromatic));
        ui->lwt_Form_2->insertItem(2,tr("色差检测结果： \t %1 %").arg(chromatic));
    }
    if(ui->rbn_Resolution->isChecked()){
        QScopedPointer<InputResolutionResult> inputresult_reso(new InputResolutionResult);
        connect(inputresult_reso.data(),&InputResolutionResult::sendResolutionResult,this,&MainWindow::get_Resolution_Result);
        inputresult_reso->exec();
        qDebug() << "分辨率分析结果：  ";

    }
    if(ui->rbn_GrayScale->isChecked()){
        QScopedPointer<InputGrayScaleResult> inputresult_gray(new InputGrayScaleResult);
        connect(inputresult_gray.data(),&InputGrayScaleResult::sendGrayScaleResult,this, &MainWindow::get_GrayScale_Result);
        inputresult_gray->exec();
        qDebug() << "灰阶分析结果：   ";
        //ui->lwt_Form_1->editItem(ui->lwt_Form_1);
    }
}

//重新选择角点，即清楚先前选择的角点数据
void MainWindow::on_btn_ReChooseAngular_clicked()
{
    emit send_ReChoseAngular();
    emit send_QuitMode();
    ui->btn_ChooseAngularPoint->setEnabled(true);
    ui->btn_AngularPointComplete->setDisabled(true);
    ui->btn_ReChooseAngular->setDisabled(true);
}

//后面是一些雷达点选择之后的界面变化
void MainWindow::on_rbn_Chromatic_clicked()
{
    on_btn_ReChooseAngular_clicked();
    ui->btn_ChooseChromatic->setEnabled(true);
    ui->btn_AngularPointComplete->setDisabled(true);
    ui->btn_ChooseAngularPoint->setDisabled(true);
    ui->btn_ReChooseAngular->setDisabled(true);
}

void MainWindow::on_rbn_Resolution_clicked()
{
    emit send_QuitMode();
    on_btn_ReChooseAngular_clicked();
    ui->btn_ChromaticComplete->setDisabled(true);
    ui->btn_ChooseChromatic->setDisabled(true);
    ui->btn_AngularPointComplete->setDisabled(true);
    ui->btn_ChooseAngularPoint->setDisabled(true);
    ui->btn_ReChooseAngular->setDisabled(true);

}

void MainWindow::on_rbn_GrayScale_clicked()
{
    emit send_QuitMode();
    on_btn_ReChooseAngular_clicked();
    ui->btn_ChromaticComplete->setDisabled(true);
    ui->btn_ChooseChromatic->setDisabled(true);
    ui->btn_AngularPointComplete->setDisabled(true);
    ui->btn_ChooseAngularPoint->setDisabled(true);
    ui->btn_ReChooseAngular->setDisabled(true);
}

void MainWindow::on_rbn_Distortion_clicked()
{
    ui->btn_ChooseAngularPoint->setEnabled(true);
    ui->btn_AngularPointComplete->setDisabled(true);
    ui->btn_ReChooseAngular->setDisabled(true);
    ui->btn_ChromaticComplete->setDisabled(true);
    ui->btn_ChooseChromatic->setDisabled(true);
}

//从其他窗口获取分辨率结果
void MainWindow::get_Resolution_Result(double temp)
{
    qDebug() << "分辨率：" << temp;
    QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("该图像的分辨率为 %1 LW/PH").arg(temp));
    resolution = temp;
    ui->lwt_Form_1->takeItem(1);
    ui->lwt_Form_2->takeItem(1);
    ui->lwt_Form_1->insertItem(1,tr("分辨率值：      \t %1 LW/PH ").arg(resolution));
    ui->lwt_Form_2->insertItem(1,tr("分辨率值：      \t %1 LW/PH").arg(resolution));
}

//从其他窗口获取灰阶分析结果
void MainWindow::get_GrayScale_Result(double temp)
{
    qDebug() << "灰阶："  << temp;
    QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("该图像的灰阶值为 %1. ").arg(temp));
    grayScale = temp;
    ui->lwt_Form_1->takeItem(4);
    ui->lwt_Form_2->takeItem(4);
    ui->lwt_Form_1->insertItem(4,tr("灰阶值：       \t %1").arg(grayScale));
    ui->lwt_Form_2->insertItem(4,tr("灰阶值：       \t %1").arg(grayScale));
}

//获取色差检测结果
double MainWindow::getChroResult(QVector<int> Chro1, QVector<int> Chro2)
{
    double youWanted = 0;
    QVector<int> result;
    for(int i = 0; i < Chro1.size(); ++i){
        int ss = Chro1[i] - Chro2[i];
        result.push_back(ss);
    }
    qDebug() << result.size() << "Chro1的size";
    for(int i = 0; i < result.size() / 3; ++i){
        double ss = pow(result[3 * i], 2) + pow(result[3 * i + 1], 2) + pow(result[3 * i + 2], 2);
        youWanted += sqrt(ss);
    }
    double nums = result.size() / 3;
    qDebug() << nums;
    youWanted = youWanted / nums;
    return youWanted;
}

/*
 * 后面这些函数都是视频质量检测的（播放器）
 */

//导入视频
void MainWindow::on_btn_ImportVideo_clicked()
{
    QString videofilepath = QFileDialog::getOpenFileName(
        this, "选择要测试的视频",
        "./",
        "视频文件 (*.mp4 *.jpg *.avi *.flv *.mov *.wmv);; 所有文件 (*.*);; ");
    if(videofilepath.isNull()){
        return;
    }

    ui->btn_goto_next_5s->setEnabled(true);
    ui->btn_goto_pre_5s->setEnabled(true);
    ui->btn_2X->setEnabled(true);
    ui->btn_Locate->setEnabled(true);
    ui->btn_NextFrame->setEnabled(true);
    ui->btn_pauseVideo->setEnabled(true);
    ui->btn_StartTestVideo->setEnabled(true);
    ui->btn_PreviousFrame->setEnabled(true);

    ui->btn_stopVideo->setEnabled(true);
    ui->btn_1X->setDisabled(true);                      //导入视频的时候常速播放为灰色
    ui->btn_ImportVideo->setDisabled(true);             //导入视频后导入视频选项为灰色
    ui->btn_resumeVideo->setDisabled(true);             //导入视频后继续播放为灰色

    //视频线程
    //构建一个获取视频数据包到队列的线程
    thread_ff = new ffmpegPlayerClass();
    mythread_1 = new QThread(this);

    thread_ff->moveToThread(mythread_1);
    //构建一个解码队列视频数据包转化为RGB图像并传输到label的线程
    thread_play = new PlayThread();
    mythread_2 = new QThread(this);
    thread_play->moveToThread(mythread_2);

    mythread_1->start();
    mythread_2->start();

    connect(this,&MainWindow::destroyed,this,&MainWindow::deal_Close);
    connect(this,&MainWindow::send_putVideoPacket,thread_ff,&ffmpegPlayerClass::putVideoPacket);
    connect(this,&MainWindow::send_getVideoPacket,thread_play,&PlayThread::decode);
    connect(thread_play,&PlayThread::send_Video,this,&MainWindow::video_imshow,Qt::BlockingQueuedConnection);
    connect(thread_play,&PlayThread::send_CurTimeAndFrame,this,&MainWindow::get_VideoCurTimeAndFrame,Qt::BlockingQueuedConnection);
    connect(thread_ff,&ffmpegPlayerClass::send_VideoPara,this,&MainWindow::get_VideoPara);
    connect(ui->sld_ProgressBar,&QSliderClass::send_changePos,[=]{
        delay(10);
        qDebug() << "进度条变化导致jump";
        int pos = ui->sld_ProgressBar->value();
        thread_play->changePos(pos);
    });
    //拖动滑块时，播放器为播放状态。
    connect(ui->sld_ProgressBar,&QSliderClass::send_moveSlide,[=]{
        ui->btn_pauseVideo->setEnabled(true);
        ui->btn_resumeVideo->setDisabled(true);
    });

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_Slider()));
    timer->start(500);

    thread_play->init();
    isdealclose = false;
    isend = false;
    emit send_putVideoPacket(videofilepath);
    while(frameFre == 0){
        delay(10);
    }
    qDebug() << "frameFre" << frameFre;
    emit send_getVideoPacket(frameFre);
    QThread::msleep(100);

}

//显示视频
void MainWindow::video_imshow(QImage temp, qint64 pts)
{
    if(temp.isNull() == true){
        return;
    }
    //    std::pair<QImage, qint64> temp_image(temp,pts);

    //    if(image_container.size() > 5){
    //        ui->lbl_Video->setPixmap(QPixmap::fromImage(image_container.front().first));

    //        qint64 yue = image_container.front().second / 1000;
    //        QString time_ = QString("%1min %2sec %3%4%5ms").arg(yue / 60).arg(yue % 60).arg(image_container.front().second % 1000 / 100).arg(image_container.front().second % 1000 % 100 /10).arg(image_container.front().second % 1000 % 10);
    //        ui->lbl_curTime->setText(time_);
    //        curTime = yue;

    //        image_container.pop_front();
    //        QThread::msleep(1);
    //    }
    //    insertContainer(image_container,temp_image);



    if(isend == false){
        ui->lbl_Video->setPixmap(QPixmap::fromImage(temp));
        qint64 yue = pts / 1000;
        QString time_ = QString("%1min %2sec %3%4%5ms").arg(yue / 60).arg(yue % 60).arg(pts % 1000 / 100).arg(pts % 1000 % 100 /10).arg(pts % 1000 % 10);
        ui->lbl_curTime->setText(time_);
        curTime = yue;
    }
}

//停止视频
void MainWindow::on_btn_stopVideo_clicked()
{

    QThread::msleep(100);
    frameFre = 0;
    //qDebug() << "dealclose运行" << "\其实是说是按下停止按钮";
    if(!thread_play->isStop()){
        thread_play->resume();
        thread_play->stop();
    }
    QThread::sleep(1);
    mythread_1->quit();
    mythread_2->quit();             //这里只是把线程退出调用，并没有释放线程

    delThread();                    //本意是想把子线程删掉，再次导入的时候再构建线程

    isdealclose = true;             //判断窗口退出之前是否有进行停止操作

    //按了停止按钮之后就传入一张黑色的图片
    QImage temp = QImage(880,660,QImage::Format_ARGB32);
    temp.fill(Qt::black);
    ui->lbl_Video->setPixmap(QPixmap::fromImage(temp));

    //设置一些按钮和数字的状态
    ui->lbl_CurFrame->setText("0");
    ui->lbl_curTime->setText("0");
    ui->lbl_TotalFrame->setText("0");
    ui->lbl_totalTime->setText("0");
    ui->sld_ProgressBar->setValue(0);

    ui->btn_ImportVideo->setEnabled(true);                  //设置导入视频选项为enable

    ui->btn_goto_next_5s->setDisabled(true);
    ui->btn_goto_pre_5s->setDisabled(true);
    ui->btn_1X->setDisabled(true);
    ui->btn_2X->setDisabled(true);
    ui->btn_Locate->setDisabled(true);
    ui->btn_NextFrame->setDisabled(true);
    ui->btn_pauseVideo->setDisabled(true);
    ui->btn_StartTestVideo->setDisabled(true);
    ui->btn_PreviousFrame->setDisabled(true);
    ui->btn_resumeVideo->setDisabled(true);
    ui->btn_stopVideo->setDisabled(true);
    ui->btn_1X->setDisabled(true);

    isend = true;                                           //标志量，用于视频停止之后的进度条呀，时间呀的置位
}

//暂停视频
void MainWindow::on_btn_pauseVideo_clicked()
{
    qDebug() << "暂停播放";
    ui->btn_pauseVideo->setDisabled(true);
    ui->btn_resumeVideo->setEnabled(true);
    thread_play->pause();
}

//摧毁窗口的时候触发的slot
void MainWindow::deal_Close()           //这里有问题！
{   //qDebug() << "是thread的stop？";
    //qDebug() << isdealclose;
    if(!isdealclose){
        if(!thread_play->isStop()){
            thread_play->stop();
        }
        mythread_1->quit();
        mythread_2->quit();
    }

    //qDebug() << "my honor";
    delay(2000);
    //qDebug() << "aa++";

    mythread_1->wait();
    //qDebug() << "aa----";
    mythread_2->wait();
    //qDebug() << "aa+++++";

    delete thread_ff;
    //qDebug() << "aaaa";
    delete thread_play;
    //qDebug() << "abbbba";
}

//继续播放
void MainWindow::on_btn_resumeVideo_clicked()
{
    ui->btn_resumeVideo->setDisabled(true);
    ui->btn_pauseVideo->setEnabled(true);
    thread_play->resume();
}

//开始检测
void MainWindow::on_btn_StartTestVideo_clicked()
{
    if(ui->rbn_FrameFreq->isChecked() == true){
        frameFre_r = frameFre;
        QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("该视频的平均帧率：%1 fps").arg(frameFre));
        ui->lwt_Form_1->takeItem(6);
        ui->lwt_Form_2->takeItem(6);
        ui->lwt_Form_1->insertItem(6,tr("帧频值：     \t %1 fps").arg(frameFre));
        ui->lwt_Form_2->insertItem(6,tr("帧频值：     \t %1 fps").arg(frameFre));
    }
    if(ui->rbn_Delayed->isChecked() == true){
        QScopedPointer<delayTesting> inputdelay(new delayTesting);
        connect(inputdelay.data(),&delayTesting::send_DelayResult,[=](qint64 a){
            qDebug() << a;
            QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("该视频的延时：%1 ms").arg(a));
            delayresu = a;
            ui->lwt_Form_1->takeItem(8);
            ui->lwt_Form_2->takeItem(8);
            ui->lwt_Form_1->insertItem(8,tr("延时值：     \t %1 ms").arg(delayresu));
            ui->lwt_Form_2->insertItem(8,tr("延时值：     \t %1 ms").arg(delayresu));
        });
        inputdelay->exec();
    }
    if(ui->rbn_SubjectiveAnalysis->isChecked() == true){
        QScopedPointer<SubjectTesting> inputsubject(new SubjectTesting);
        connect(inputsubject.data(),&SubjectTesting::send_SubjectResu,[=](bool fluency,bool joggle, bool disturb, bool masic){
            isfluency = fluency;
            isjoggle = joggle;
            isdisturb = disturb;
            ismasic = masic;
            QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("您已完成主观评价."));
            importSubjectResult();
        });
        inputsubject->exec();
    }
    if(ui->rbn_CodeRate->isChecked() == true){
        QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("该视频的平均码率：%1 kb/s").arg(codeRate));
        codeRate_r = codeRate;
        ui->lwt_Form_1->takeItem(7);
        ui->lwt_Form_2->takeItem(7);
        ui->lwt_Form_1->insertItem(7,tr("码率值：     \t %1 kb/s").arg(codeRate));
        ui->lwt_Form_2->insertItem(7,tr("码率值：     \t %1 kb/s").arg(codeRate));
    }
}

//输入主观检测结果
void MainWindow::importSubjectResult()
{
    ispassSub = true;

    ui->lwt_Form_1->takeItem(11);
    ui->lwt_Form_2->takeItem(11);

    if(isfluency){
        ui->lwt_Form_1->insertItem(11,tr("流畅性不足：     \t 是"));
        ui->lwt_Form_2->insertItem(11,tr("流畅性不足：     \t 是"));
    }else{
        ui->lwt_Form_1->insertItem(11,tr("流畅性不足：     \t 是"));
        ui->lwt_Form_2->insertItem(11,tr("流畅性不足：     \t 是"));
    }

    ui->lwt_Form_1->takeItem(9);
    ui->lwt_Form_2->takeItem(9);
    if(isdisturb){
        ui->lwt_Form_1->insertItem(9,tr("干扰：         \t 是"));
        ui->lwt_Form_2->insertItem(9,tr("干扰：         \t 是"));
    }else{
        ui->lwt_Form_1->insertItem(9,tr("干扰：         \t 否"));
        ui->lwt_Form_2->insertItem(9,tr("干扰：         \t 否"));
    }

    ui->lwt_Form_1->takeItem(10);
    ui->lwt_Form_2->takeItem(10);
    if(ismasic){
        ui->lwt_Form_1->insertItem(10,tr("马赛克现象：   \t 是"));
        ui->lwt_Form_2->insertItem(10,tr("马赛克现象：   \t 是"));
    }else{
        ui->lwt_Form_1->insertItem(10,tr("马赛克现象：   \t 否"));
        ui->lwt_Form_2->insertItem(10,tr("马赛克现象：   \t 否"));
    }

    ui->lwt_Form_1->takeItem(12);
    ui->lwt_Form_2->takeItem(12);
    if(isjoggle){
        ui->lwt_Form_1->insertItem(12,tr("抖动：        \t\t 是"));
        ui->lwt_Form_2->insertItem(12,tr("抖动：        \t\t 是"));
    }else{
        ui->lwt_Form_1->insertItem(12,tr("抖动：        \t\t 否"));
        ui->lwt_Form_2->insertItem(12,tr("抖动：        \t\t 否"));
    }
}

//ms级别的延时函数---会阻塞当前线程
void MainWindow::delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//用于pts的调序（已禁用）
void MainWindow::insertContainer(std::deque<std::pair<QImage, qint64>> &myque, std::pair<QImage, qint64> unit_)
{
    //用作动态存储器
    if(myque.empty() == true){
        myque.push_back(unit_);
        return;
    }
    if(unit_.second < myque.front().second){
        myque.push_front(unit_);
        return;
    }
    if(unit_.second > myque.back().second){
        myque.push_back(unit_);
        return;
    }
    auto pos = myque.begin();
    for(auto i = myque.begin(); i != myque.end(); ++i){
        if((i+1) != myque.end()){
            if(unit_.second > i->second && unit_.second < (i+1)->second){
                pos = i;
                break;
            }
        }
    }
    myque.emplace(++pos,unit_);
}

//更新进度条的slot--0.5s调用一次，只有当进度条位置变化超过1/200才进行更新
void MainWindow::update_Slider()
{
    if(isend){
        return;
    }
    double slider_scale = curTime / totalTime;
    double temp = curTime_scale - slider_scale;
    if(ui->sld_ProgressBar->mouseDrag() == false && (temp >= 0.005 || temp <= -0.005)){
        QThread::msleep(1);
        curTime_scale = slider_scale;
        qDebug() << slider_scale << "slider占比";
        ui->sld_ProgressBar->setValue(slider_scale * 200);
    }
}

//获取全部暂时能知道的视频信息
void MainWindow::get_VideoPara(qint64 _totalTime, int _frameFre, int _codeRate, int _totalFrame, double _time_factor)
{
    QThread::msleep(100);
    qint64 usec = _totalTime / 1000000;
    qint64 ms = _totalTime % 1000000 / 1000;
    QString temp = QString("%1 min  %2 sec %3 ms").arg(usec/60).arg(usec%60).arg(ms);
    qDebug() << "获得视频信息";
    totalFrame = _totalFrame;
    totalTime = static_cast<double>(_totalTime / 1000000);
    totalTime_QS = temp;
    frameFre = _frameFre;
    codeRate = _codeRate;
    time_factor = _time_factor;
    ui->lbl_TotalFrame->setText(QString::number(totalFrame));
    ui->lbl_totalTime->setText(temp);
}

//获取当前的时间和帧
void MainWindow::get_VideoCurTimeAndFrame(qint64 time, int curFrame)
{
    if(isend){
        return;
    }
    int frame = time / (1000 / frameFre);
    if(frame >= totalFrame){
        //如果帧数大于总帧数，就进行一个小操作，将slider置位200，当前帧数变成最大帧数，停止播放，并且将isend置位为true
        ui->sld_ProgressBar->setValue(200);
        on_btn_stopVideo_clicked();
        isend = true;
        ui->lbl_curTime->setText(totalTime_QS);
    }
    ui->lbl_CurFrame->setText(QString::number(frame));
    QThread::msleep(0);
}

//显示下一帧
void MainWindow::on_btn_NextFrame_clicked()
{
    //image_container.clear();                    //清空动态存储器的元素
    ui->btn_resumeVideo->setEnabled(true);
    ui->btn_pauseVideo->setDisabled(true);
    thread_play->getNextFrame();
}

//显示上一帧
void MainWindow::on_btn_PreviousFrame_clicked()
{
    //image_container.clear();                    //清空动态存储器的元素
    ui->btn_resumeVideo->setEnabled(true);
    ui->btn_pauseVideo->setDisabled(true);
    thread_play->getPreFrame();
}

//跳到之前30帧
void MainWindow::on_btn_goto_pre_5s_clicked()
{
    ui->btn_resumeVideo->setEnabled(true);
    ui->btn_pauseVideo->setDisabled(true);
    thread_play->gotoPre1sec();
}

//跳到之后30帧
void MainWindow::on_btn_goto_next_5s_clicked()
{
    ui->btn_resumeVideo->setEnabled(true);
    ui->btn_pauseVideo->setDisabled(true);
    thread_play->gotoNext1sec();
}

//测试用接口--跳到115000的pts位置
void MainWindow::on_btn_DefineHead_clicked()
{
    thread_play->gotoTest();
}

//0.5倍数播放
void MainWindow::on_btn_2X_clicked()
{
    ui->btn_2X->setDisabled(true);
    ui->btn_1X->setEnabled(true);
    thread_play->slowVel();
}

//常数播放
void MainWindow::on_btn_1X_clicked()
{
    ui->btn_1X->setDisabled(true);
    ui->btn_2X->setEnabled(true);
    thread_play->normalVel();
}

//定位
void MainWindow::on_btn_Locate_clicked()
{
    ui->btn_pauseVideo->setDisabled(true);
    ui->btn_resumeVideo->setEnabled(true);
    QString num = ui->let_LocateFrameNums->text();
    int frame = num.toInt();
    if(frame > totalFrame || frame <= 0){
        qDebug() << "定位的帧不存在";
        QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("定位失败，该帧不存在！"));
        return;
    }
    thread_play->gotoPos(frame, frameFre);
}

//提交表格
void MainWindow::on_btn_HandIn_clicked()
{
    //将所有记录的数据转化成QVector<QString>
    bool isOver = true;
    QVector<QString> list;
    if(resolution != -1000){
        list.push_back(QString("分辨率值： %1 LW/PH").arg(resolution));
    }else{
        list.push_back(QString("未检测"));
        isOver = false;
    }
    if(chromatic != -1000){
        list.push_back(QString("色差： %1").arg(chromatic));
    }else{
        list.push_back(QString("未检测"));
        isOver = false;
    }
    if(distort != -1000){
        list.push_back(QString("失真： %1").arg(distort));
    }else{
        list.push_back(QString("未检测"));
        isOver = false;
    }

    if(grayScale != -1000){
        list.push_back(QString("灰阶值： %1").arg(grayScale));
    }else{
        list.push_back(QString("未检测"));
        isOver = false;
    }

    if(frameFre_r != -1000){
        list.push_back(QString("帧频值： %1 fps").arg(frameFre_r));
    }else{
        list.push_back(QString("未检测"));
        isOver = false;
    }


    if(codeRate_r != -1000){
        list.push_back(QString("码率值：%1 kb/s").arg(codeRate_r));
        qDebug() << "codeRate";
        qDebug() << codeRate;
        qDebug() << "skk";
    }else{
        list.push_back(QString("未检测"));
        isOver = false;
    }

    if(delayresu != INT_MIN){
        list.push_back(QString("延时值： %1 ms").arg(delayresu));
    }else{
        list.push_back(QString("未检测"));
        isOver = false;
    }

    if(!ispassSub){
        list.push_back(QString("干扰未检测"));
        list.push_back(QString("马赛克未检测"));
        list.push_back(QString("流畅性未检测"));
        list.push_back(QString("抖动未检测"));
        isOver = false;
    }else{
        if(isdisturb){
            list.push_back(QString("有干扰"));
        }else{
            list.push_back(QString("无干扰"));
        }

        if(ismasic){
            list.push_back(QString("有马赛克"));
        }else{
            list.push_back(QString("无马赛克"));
        }

        if(isfluency){
            list.push_back(QString("视频流畅"));
        }else{
            list.push_back(QString("流畅性不足"));
        }

        if(isjoggle){
            list.push_back(QString("有抖动"));
        }else{
            list.push_back(QString("无抖动"));
        }
    }

    if(isOver == false){
        QMessageBox msg(this);//对话框设置父组件
        msg.setWindowTitle("提示");//对话框标题
        msg.setText("尚未完成所有检测，确定提交表单？");//对话框提示文本
        msg.setIcon(QMessageBox::Information);//设置图标类型
        msg.setStandardButtons(QMessageBox::Ok | QMessageBox:: Cancel );//对话框上包含的按钮

        if(msg.exec() == QMessageBox::Cancel)//模态调用
        {
            return;
        }
    }

    QScopedPointer<inputYourName> input_yourname(new inputYourName);
    connect(input_yourname.data(),&inputYourName::send_testingName,this,&MainWindow::get_Your_Name);
    input_yourname->exec();

    list.push_back(testingN);
    list.push_back(testerN);
    list.push_back(modelN);

    QScopedPointer<makeExcle> make_excel (new makeExcle);
    make_excel.data()->new_Excel(list);

    QMessageBox::information(nullptr, "机器人视觉质量检测", QStringLiteral("已生成Excel表单"));
    //重置一些数据，方便下一次重新提交
    reSetExcel();
    reSetLwtForm();
}

//从其他窗口获取名字信息
void MainWindow::get_Your_Name(QString s1, QString s2, QString s3)
{
    testerN = s2;
    testingN = s1;
    modelN = s3;
}

//删除线程实例操作，用于停止视频按钮
void MainWindow::delThread()
{
    ffmpegPlayerClass* temp_ff;
    PlayThread* temp_play;
    ffmpegPlayerClass* ss = thread_ff;
    PlayThread* yy = thread_play;
    thread_ff = temp_ff;
    thread_play = temp_play;
    delete ss;
    delete yy;
    qDebug() << "如果顺利则已经删除线程";
}
