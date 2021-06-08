#include "inputresolutionresult.h"
#include "ui_inputresolutionresult.h"

InputResolutionResult::InputResolutionResult(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputResolutionResult)
{
    ui->setupUi(this);
    qDebug() << "分辨率窗口构造";
}

InputResolutionResult::~InputResolutionResult()
{
    qDebug() << "分辨率窗口析构";
    delete ui;
}

void InputResolutionResult::on_buttonBox_accepted()
{
    emit sendResolutionResult(ui->lineEdit->text().toDouble());
}
