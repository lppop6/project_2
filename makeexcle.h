#ifndef MAKEEXCLE_H
#define MAKEEXCLE_H


//QT += axcontainer只能在Win下使用，Linux下只能在Wine上使用。这块估计要用Win来写了。
#include <QAxObject>
#include <QVector>
#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>
#include <QDebug>
#include <QDateTime>

enum EXcel_ColumnType{
    ColumnA = 1,
    ColumnB = 2,
    ColumnC = 3,
    ColumnD = 4,
    ColumnE = 5,
    ColumnF = 6,
    ColumnG = 7,
    ColumnH = 8,
    ColumnI = 9
};

class makeExcle
{
public:
    makeExcle();

public:
    void new_Excel(QVector<QString> list);
    QString saveAs();
    void setCell(QAxObject *worksheet,EXcel_ColumnType column,int row,QColor color,QString text);
};

#endif // MAKEEXCLE_H
