#include "makeexcle.h"

makeExcle::makeExcle()
{
    qDebug() << "makeExcel构造";
}

//根据模板生成excel
void makeExcle::new_Excel(QVector<QString> list)
{
    QString templatePath = "./template.xlsx";
    QFileInfo info(templatePath);

    if(!info.exists())
    {
        qDebug()<<"template.xlsx is NULL";
        return;
    }

    templatePath = info.absoluteFilePath();                   //获取模板的绝地路径
    templatePath = QDir::toNativeSeparators(templatePath);   //转换一下路径,让windows能够识别

    QString ExcelFile = QDir::toNativeSeparators(saveAs());  //打开文件保存对话框,找到要保存的位置

    if(ExcelFile=="")
        return ;

    QFile::copy(templatePath, ExcelFile);                   //将模板文件复制到要保存的位置去

    info.setFile(ExcelFile);
    info.setFile(info.dir().path()+"/~$"+info.fileName());

    if(info.exists())          //判断一下,有没有"~$XXX.xlsx"文件存在,是不是为只读
    {
        qDebug()<<"报表属性为只读,请检查文件是否已打开!";
        return ;
    }


    QAxObject *excel = new QAxObject();//建立excel操作对象
    excel->setControl("Excel.Application");//连接Excel控件
    excel->setProperty("Visible", true);//显示窗体看效果
    excel->setProperty("DisplayAlerts", false);//显示警告看效果
    QAxObject *workbooks = excel->querySubObject("WorkBooks");

    QAxObject* workbook = workbooks->querySubObject("Open(const QString&)",QDir::toNativeSeparators(ExcelFile) ); //打开

    excel->setProperty("Caption", "视觉评价记录");
    QAxObject *work_book = excel->querySubObject("ActiveWorkBook");

    QAxObject *worksheet = work_book->querySubObject("Sheets(int)",1);     //获取表单1

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd");

    setCell(worksheet,ColumnE,2,Qt::black, current_date);

    /* 0~10为测试结果，11~13为被测试人员、测试人员和产品型号 */
    setCell(worksheet,ColumnC,6,Qt::black,list[0]);

    setCell(worksheet,ColumnC,7,Qt::black,list[1]);

    setCell(worksheet,ColumnC,8,Qt::black,list[2]);

    setCell(worksheet,ColumnC,9,Qt::black,list[3]);

    setCell(worksheet,ColumnC,11,Qt::black,list[4]);

    setCell(worksheet,ColumnC,12,Qt::black,list[5]);

    setCell(worksheet,ColumnC,13,Qt::black,list[6]);

    setCell(worksheet,ColumnC,14,Qt::black,list[7]);

    setCell(worksheet,ColumnC,15,Qt::black,list[8]);

    setCell(worksheet,ColumnC,16,Qt::black,list[9]);

    setCell(worksheet,ColumnC,17,Qt::black,list[10]);

    setCell(worksheet,ColumnB,1,Qt::black,list[11]);

    setCell(worksheet,ColumnB,2,Qt::black,list[12]);

    setCell(worksheet,ColumnE,1,Qt::black,list[13]);

    workbook->dynamicCall("Save()" );


    workbook->dynamicCall("Close()");  //关闭文件
    excel->dynamicCall("Quit()");//关闭excel
}

//保存位置设置
QString makeExcle::saveAs()
{
    QString file;
    QString filter;


    //如果版本低于QT5,则需要将:
    //  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
    //改为:QDesktopServices::storageLocation(QDesktopServices::DesktopLocation),
    file = QFileDialog::getSaveFileName (
        NULL,                               //父组件
        "另存为",                              //标题
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),                 //设置路径, .表示当前路径,./表示更目录
        "Excel(*.xlsx)",     //过滤器
        &filter  );

    return file;
}

//设置excel的单元格
void makeExcle::setCell(QAxObject *worksheet, EXcel_ColumnType column, int row, QColor color, QString text)
{
    QAxObject *cell = worksheet->querySubObject("Cells(int,int)", row, column);
    cell->setProperty("Value", text);
    QAxObject *font = cell->querySubObject("Font");
    font->setProperty("Color", color);
}
