#include "delaytesting.h"
#include "ui_delaytesting.h"

delayTesting::delayTesting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::delayTesting)
{
    ui->setupUi(this);
}

delayTesting::~delayTesting()
{
    delete ui;
}

void delayTesting::on_buttonBox_accepted()
{
    qint64 temp_m = ui->let_time->text().toInt();
    qint64 temp_v_0 = ui->let_time_v_0->text().toInt();
    qint64 temp_v_now = ui->let_time_v_now->text().toInt();
    emit send_DelayResult(temp_m - temp_v_now + temp_v_0);
}

void delayTesting::on_buttonBox_rejected()
{
    this->deleteLater();
}
