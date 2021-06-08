#include "imageprocessingclass.h"

ImageProcessingClass::ImageProcessingClass(QObject *parent) : QObject(parent)
{
    qDebug() << "ImageProcessingClass构造";
}

ImageProcessingClass::~ImageProcessingClass()
{
    qDebug() << "ImageProcessClass析构";
}

//首先将temp根据qv的值分为4块Qimage，然后将这四块QImage转化成Mat，之后二值化，二值化之后计算顶点的像素点位置，转化为QImage的位置，进行计算畸变数值
double ImageProcessingClass::getQImageFromLabel_Dis(QImage temp, QVector<int> qv)
{
    //qv是先放pre后放cur，先放x后放y。
    double result = -1;
    QVector<QImage> myvec = imagePartition(temp,qv);
    QVector<int> position;
    QVector<int> realposition;
    QVector<int> midposition;

    //计算最大的y值和最小的y值，进而缩小对中点的坐标搜索范围，这个最大的y值和最小的y值是由选取的左顶点确定的。
    int htop = qv[1];
    int hbottom = qv[1];
    for(int i = 0; i < 4; i ++){
        int tt = qv[4 * i + 2];
        if(tt <  htop){
            htop = tt;
        }
        if(hbottom < tt ){
            hbottom = tt;
        }
        tt = qv[4 * i + 3];
        if(tt <  htop){
            htop = tt;
        }
        if(hbottom < tt ){
            hbottom = tt;
        }
    }

    qDebug() << "这是pv里面的数值";
    for(auto i : qv){
        qDebug() << i;
    }
    qDebug() << "结束pv";

    //获得顶点再分割后的图片位置
    for(int i = 0; i < 4; ++i){
        int position_x = 0;
        int position_y = 0;

        //转化为二值图像
        cv::Mat test = qImageToMat(myvec[i]);
        cv::cvtColor(test,test,cv::COLOR_RGB2GRAY);
        cv::threshold(test,test,150,255,cv::THRESH_BINARY);

        //计算顶点坐标，保存到position数组里面
        qDebug() << "这是第" << i << "次进行计算" ;
        getPosition_Dis(test,i,position_x,position_y);
        position.push_back(position_x);
        position.push_back(position_y);
        qDebug() << "小块里的x: " << position_x;
        qDebug() << "小块里的y: " << position_y;
        //image显示
        //        cv::imshow("ss",test);
        //        cv::waitKey(1000);
    }

    //转化为原始图片的位置
    for(int i = 0; i < 4; ++i){
        int x = position[2 * i];
        int y = position[2 * i + 1];
        x = qv[4 * i] + x;
        y = qv[4 * i + 2] + y;
        realposition.push_back(x);
        realposition.push_back(y);
    }

    qDebug() << "..........";
    qDebug() << "输出真实坐标：";
    for(auto i : realposition){
        qDebug() << i;
    }
    qDebug() << "...........";




    qDebug() << "position  size:  " << position.size();
    //此时我们已经获得了4个顶点的坐标，那么，我们就要开始计算两个中点的坐标
    midposition = getMidPoint(temp,realposition,htop,hbottom);

    //获得了所有的坐标，现在开始计算畸变率
    double A1 = getDistance(realposition[0],realposition[1],realposition[6],realposition[7]);
    qDebug() << "A1:   " << A1;
    double A2 = getDistance(realposition[2],realposition[3],realposition[4], realposition[5]);
    qDebug() << "A2:   " << A2;
    double B = getDistance(midposition[0],midposition[1], midposition[2],midposition[3]);
    result = ((A1+A2) / 2.0 - B) / B;
    qDebug() << "B：   " << B;
    qDebug() << "result:   " << result;


    cv::Mat test = qImageToMat(temp);
    cv::cvtColor(test,test,cv::COLOR_RGB2GRAY);
    cv::threshold(test,test,150,255,cv::THRESH_BINARY);
    //imshow("test", test);
    qDebug() << "畸变分析！！";
    return result;
}

//QImage转换Mat
cv::Mat ImageProcessingClass::qImageToMat(QImage tmp)
{
    cv::Mat result;
    switch(tmp.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        result = cv::Mat(tmp.height(), tmp.width(), CV_8UC4, (void*)tmp.constBits(), tmp.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        result = cv::Mat(tmp.height(), tmp.width(), CV_8UC3, (void*)tmp.constBits(), tmp.bytesPerLine());
        cv::cvtColor(result, result, cv::COLOR_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        result = cv::Mat(tmp.height(), tmp.width(), CV_8UC1, (void*)tmp.constBits(), tmp.bytesPerLine());
        break;
    }
    //    cvtColor(result,result,COLOR_RGB2GRAY);
    //    threshold(result,result,100,255,THRESH_BINARY);
    //    qDebug() << "distort处理过了";
    //    imshow("ss",result);
    return result;
}

//根据辅助选取的点分割图像依次进行检测，这个是用于角点
QVector<QImage> ImageProcessingClass::imagePartition(QImage temp, QVector<int> qv)
{
    //qv中 0～3分别是 prex、curx、prey、cury，后面依次类推 4～7 ....
    QImage part1 = temp.copy(qv[0], qv[2],qv[1] - qv[0],qv[3] - qv[2]);
    QImage part2 = temp.copy(qv[4], qv[6], qv[5] - qv[4], qv[7] - qv[6]);
    QImage part3 = temp.copy(qv[8], qv[10],qv[9] - qv[8],qv[11] - qv[10]);
    QImage part4 = temp.copy(qv[12], qv[14], qv[13] - qv[12], qv[15] - qv[14]);

    QVector<QImage> result;
    result.push_back(part1);
    result.push_back(part2);
    result.push_back(part3);
    result.push_back(part4);

    return result;
}

//获取两点之间的距离
int ImageProcessingClass::getDistance(int x1, int y1, int x2, int y2)
{
    int result;
    result = pow(x1 - x2, 2) + pow(y1 - y2, 2);
    return result;
}

//计算四个顶点位置，通过style来分别计算，style：0～3分别是左上、右上、右下、左下
void ImageProcessingClass::getPosition_Dis(cv::Mat temp, int style, int & position_x, int & position_y)
{
    int dis = 0;                        //到黑色像素点到原点之间的距离
    int min_dis = INT_MAX;              //记录最小距离
    qDebug() << "temp size:" << temp.size;
    switch (style) {
    case 0:
        for(int i = 0; i < temp.rows; ++i){
            uchar* data = temp.ptr<uchar>(i);
            for(int j = 0; j < temp.cols; ++j){
                if(data[j] == 0){
                    dis = getDistance(j ,i ,0 ,0 );
                    if(min_dis >= dis ){
                        min_dis = dis;
                        position_y = i;
                        position_x = j;
                        qDebug() << "你好";
                        qDebug() << i << j;
                        qDebug() << "....";
                    }
                    break;
                }
            }
        }
        qDebug() << "这里是左上";
        qDebug() << dis << "disdis";
        qDebug() << position_x << "xxx";
        qDebug() << position_y << "yyy";
        break;
    case 1:
        for(int i = 0; i < temp.rows; ++i){
            uchar* data = temp.ptr<uchar>(i);
            for(int j = temp.cols - 1; j > 0; --j){
                if(data[j] == 0){
                    dis = getDistance(j ,i ,temp.cols ,0);
                    if(min_dis >= dis ){
                        min_dis = dis;
                        position_y = i;
                        position_x = j;
                        qDebug() << "你好";
                        qDebug() << i << j;
                        qDebug() << "....";
                    }
                    break;
                }
            }
        }
        qDebug() << "这里是右上";
        qDebug() << dis << "disdis";
        qDebug() << position_x << "xxx";
        qDebug() << position_y << "yyy";
        break;
    case 2:
        for(int i = temp.rows - 1; i > 0; --i){
            uchar* data = temp.ptr<uchar>(i);
            for(int j = temp.cols - 1; j > 0; --j){
                if(data[j] == 0){
                    dis = getDistance(j ,i ,temp.cols , temp.rows );
                    if(min_dis >= dis ){
                        min_dis = dis;
                        position_y = i;
                        position_x = j;
                        qDebug() << "你好";
                        qDebug() << i << j;
                        qDebug() << "....";
                    }
                    break;
                }
            }
        }
        qDebug() << "这里是右下";
        qDebug() << dis << "disdis";
        qDebug() << position_x << "xxx";
        qDebug() << position_y << "yyy";

        break;
    case 3:
        for(int i = temp.rows - 1; i > 0; --i){
            uchar* data = temp.ptr<uchar>(i);
            for(int j = 0; j < temp.cols - 1; ++j){
                if(data[j] == 0){
                    dis = getDistance(j ,i ,0 ,temp.rows );
                    if(min_dis > dis ){
                        min_dis = dis;
                        position_y = i;
                        position_x = j;
                        qDebug() << "你好";
                        qDebug() << i << j;
                        qDebug() << "....";
                    }
                    break;
                }
            }
        }
        qDebug() << "这里是左下";
        qDebug() << dis << "disdis";
        qDebug() << position_x << "xxx";
        qDebug() << position_y << "yyy";
        break;
    default:
        position_x = -1;
        position_y = -1;
        break;
    }
}

//scope保存着中点搜索范围的左上点 x1 y1，右下点 x2 y2
QVector<int> ImageProcessingClass::getMidPoint(QImage temp, QVector<int> qv,int top , int bottom)
{
    QVector<int> result;
    int topMid = 0;
    int bottomMid = 0;

    topMid = (qv[0] + qv[2]) / 2;
    bottomMid = (qv[4] + qv[6]) / 2;

    qDebug() << "topMid" << topMid;
    qDebug() << "bottomMid" << bottomMid;
    qDebug() << top ;
    qDebug() << bottom;
    qDebug() << "000000000";

    cv::Mat tempMat = qImageToMat(temp);
    cv::cvtColor(tempMat,tempMat,cv::COLOR_RGB2GRAY);
    cv::threshold(tempMat,tempMat,150,255,cv::THRESH_BINARY);


    for(int i = top; i < bottom; ++i){
        uchar* data = tempMat.ptr<uchar>(i);
        if(data[topMid] == 0){
            qDebug() << "1111111111111";
            result.push_back(topMid);
            result.push_back(i);
            break;
        }
    }

    for(int i = bottom; i > top; --i){
        uchar* data = tempMat.ptr<uchar>(i);
        if(data[bottomMid] == 0){
            qDebug() << "22222222222222";
            result.push_back(bottomMid);
            result.push_back(i);
            break;
        }
    }
    qDebug() << "计算出的两个中心点：" << result[0] << result[1] << "......" <<result[2] << result[3];
    return result;
}

//从图片中获取色差检测的lab值
QVector<int> ImageProcessingClass::getQImageFromLabel_Chro(QImage temp, QVector<int> qv)
{
    QVector<int> result;
    QVector<QImage> myvec = splitMatToChro24(temp,qv);
    cv::Mat test;
    std::vector<cv::Mat> mv;
    for(int i = 0; i < 24; ++i){
        test = qImageToMat(myvec[i]);
        //        imshow("ss", test);
        //        cv::waitKey(1000);
        cv::cvtColor(test,test,cv::COLOR_RGB2Lab);
        cv::split(test, mv);
        //qDebug() << "L空间" << mv[0];
        //        uchar* data1 = mv[1].ptr<uchar>(1);
        //        qDebug() << data1[1] << "a空间";
        //        uchar* data0 = mv[0].ptr<uchar>(1);
        //        qDebug() << data0[1] << "L空间";
        //        uchar* data3 = mv[2].ptr<uchar>(1);
        //        qDebug() << data3[0] << "b空间";
        int L = getAverageChro(mv[0]);
        int a = getAverageChro(mv[1]);
        int b = getAverageChro(mv[2]);
        qDebug() << "函数式结果 L" << L;
        qDebug() << "函数式结果 a" << a;
        qDebug() << "函数式结果 b" << b;
        result.push_back(L);
        result.push_back(a);
        result.push_back(b);
        //        imshow("mv0",mv[0]);            //L空间
        //        imshow("mv1",mv[1]);            //a空间
        //        imshow("mv2",mv[2]);            //b空间
        //        cv::waitKey(3000);
    }

    //cv::imshow(test,"ss");
    qDebug() << "色差分析！！";
    return result;
}

//根据辅助选择的点来将色差图卡图片转换成24个检测区域
QVector<QImage> ImageProcessingClass::splitMatToChro24(QImage orginPic,QVector<int> qs)
{
    QVector<QImage> result;
    double distance_x = (qs[1] - qs[0]) / 38.0;
    double distance_y = (qs[3] - qs[2]) / 26.0;
    double x = qs[0] + 2 * distance_x;
    double y = qs[2] + 2 * distance_y;
    double w = distance_x * 4;
    double h = distance_y * 4;
    for(int i = 0; i < 4; ++i){
        for(int j = 0; j < 6; ++j){
            QImage temp = orginPic.copy(x,y,w,h);
            result.push_back(temp);
            x = x + distance_x * 6;
        }
        y = y + distance_y * 6;
        x = qs[0] + 2 * distance_x;
    }
    return result;
}

//计算平均色差
int ImageProcessingClass::getAverageChro(cv::Mat temp)
{
    qint64 st = 0;
    int result;
    int total = temp.rows;
    for(int i = 0; i < temp.rows - 1; ++i){
        uchar* data = temp.ptr<uchar>(i);
        st += data[temp.cols / 2];
    }
    result = st / total;
    return result;
}
