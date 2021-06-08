#include "inputgrayscaleresult.h"
#include "ui_inputgrayscaleresult.h"

InputGrayScaleResult::InputGrayScaleResult(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputGrayScaleResult)
{
    ui->setupUi(this);
}

InputGrayScaleResult::~InputGrayScaleResult()
{
    delete ui;
}

void InputGrayScaleResult::on_buttonBox_accepted()
{
    emit sendGrayScaleResult(ui->lineEdit->text().toDouble());
}
