#ifndef IMAGEPROCESSINGCLASS_H
#define IMAGEPROCESSINGCLASS_H

#include <QObject>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <QVector>
#include <QImage>
#include <QDebug>

class ImageProcessingClass : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessingClass(QObject *parent = nullptr);
    ~ImageProcessingClass();
public:
    double getQImageFromLabel_Dis(QImage,QVector<int>);                //接受从ImageLabelClass中传递来的QImage和vector，这用于计算畸变率
    QVector<int> getQImageFromLabel_Chro(QImage,QVector<int>);               //接受从ImageLabelClass中传递来的QImage和vector，这用于计算色差值
    cv::Mat qImageToMat(QImage);                                       //将QImage 转化成 Mat
    QVector<QImage> imagePartition(QImage,QVector<int>);
    int getDistance(int ,int ,int ,int );                              //计算两点之间的距离
    void getPosition_Dis(cv::Mat,int,int&,int&);                       //第一个int是判断是哪个顶点。后面的int是返回结果。
    QVector<int> getMidPoint(QImage,QVector<int>,int,int);             //计算两个中点的坐标，第一个QImage是原图，第二个QVector是保存着四个顶点的原图坐标
        //后面的int保存着中点的搜索范围，由四个小方块图片位置确定。
    QVector<QImage> splitMatToChro24(QImage,QVector<int>);              //将Mat图像按照畸变的选取的点选取24个色块
    int getAverageChro(cv::Mat);                                        //用于计算Lab色域空间均值
private:
    cv::Mat distort;
    cv::Mat chromatic;

};

#endif // IMAGEPROCESSINGCLASS_H
